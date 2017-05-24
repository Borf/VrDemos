#include <Windows.h>
#include <NuiApi.h>
#ifndef NOKINECT
#pragma comment(lib, "Kinect10.lib")
#include "KinectDemo.h"

#include <VrLib/Log.h>
#include <windows.h>
#include <gl/glew.h>
#include <glm/gtc/type_ptr.hpp>



using vrlib::logger;
using vrlib::Log;


KinectDemo::KinectDemo(void) : Demo("Kinect")
{
}


KinectDemo::~KinectDemo(void)
{
}

// Current Kinect
INuiSensor*             m_pNuiSensor;
HANDLE                  m_pDepthStreamHandle;
HANDLE                  m_hNextDepthFrameEvent;
static const int        cDepthWidth = 640;
static const int        cDepthHeight = 480;

float*					depthMap = new float[640*480];




/// <summary>
/// Create the first connected Kinect found 
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT CreateFirstConnected()
{
	INuiSensor * pNuiSensor;
	HRESULT hr;

	int iSensorCount = 0;
	hr = NuiGetSensorCount(&iSensorCount);
	if (FAILED(hr))
	{
		return hr;
	}

	// Look at each Kinect sensor
	for (int i = 0; i < iSensorCount; ++i)
	{
		// Create the sensor so we can check status, if we can't create it, move on to the next
		hr = NuiCreateSensorByIndex(i, &pNuiSensor);
		if (FAILED(hr))
		{
			continue;
		}

		// Get the status of the sensor, and if connected, then we can initialize it
		hr = pNuiSensor->NuiStatus();
		if (S_OK == hr)
		{
			m_pNuiSensor = pNuiSensor;
			break;
		}

		// This sensor wasn't OK, so release it since we're not using it
		pNuiSensor->Release();
	}

	if (NULL != m_pNuiSensor)
	{
		// Initialize the Kinect and specify that we'll be using depth
		hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH);
		if (SUCCEEDED(hr))
		{
			// Create an event that will be signaled when depth data is available
			m_hNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

			// Open a depth image stream to receive depth frames
			hr = m_pNuiSensor->NuiImageStreamOpen(
				NUI_IMAGE_TYPE_DEPTH,
				NUI_IMAGE_RESOLUTION_640x480,
				0,
				2,
				m_hNextDepthFrameEvent,
				&m_pDepthStreamHandle);
		}
	}

	if (NULL == m_pNuiSensor || FAILED(hr))
	{
		throw("No ready Kinect found!");
		return E_FAIL;
	}

	return hr;
}


/// <summary>
/// Handle new depth data
/// </summary>
void ProcessDepth()
{
	HRESULT hr;
	NUI_IMAGE_FRAME imageFrame;

	// Attempt to get the depth frame
	hr = m_pNuiSensor->NuiImageStreamGetNextFrame(m_pDepthStreamHandle, 0, &imageFrame);
	if (FAILED(hr))
	{
		return;
	}

	BOOL nearMode;
	INuiFrameTexture* pTexture;

	// Get the depth image pixel texture
	hr = m_pNuiSensor->NuiImageFrameGetDepthImagePixelFrameTexture(
		m_pDepthStreamHandle, &imageFrame, &nearMode, &pTexture);
	if (FAILED(hr))
	{
		goto ReleaseFrame;
	}

	NUI_LOCKED_RECT LockedRect;

	// Lock the frame data so the Kinect knows not to modify it while we're reading it
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	// Make sure we've received valid data
	if (LockedRect.Pitch != 0)
	{
		// Get the min and max reliable depth for the current frame
		int minDepth = (nearMode ? NUI_IMAGE_DEPTH_MINIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MINIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;
		int maxDepth = (nearMode ? NUI_IMAGE_DEPTH_MAXIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MAXIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;

		const NUI_DEPTH_IMAGE_PIXEL * pBufferRun = reinterpret_cast<const NUI_DEPTH_IMAGE_PIXEL *>(LockedRect.pBits);

		// end pixel is start + width*height - 1
		const NUI_DEPTH_IMAGE_PIXEL * pBufferEnd = pBufferRun + (cDepthWidth * cDepthHeight);

		float* depthPointer = depthMap;

		while (pBufferRun < pBufferEnd)
		{
			// discard the portion of the depth that contains only the player index
			USHORT depth = pBufferRun->depth;

			// To convert to a byte, we're discarding the most-significant
			// rather than least-significant bits.
			// We're preserving detail, although the intensity will "wrap."
			// Values outside the reliable depth range are mapped to 0 (black).

			// Note: Using conditionals in this loop could degrade performance.
			// Consider using a lookup table instead when writing production code.
			//BYTE intensity = static_cast<BYTE>(depth >= minDepth && depth <= maxDepth ? depth % 256 : 0);

			*(depthPointer++) = depth / 500.0;
			// Increment our index into the Kinect's depth buffer
			++pBufferRun;
		}
	}

	// We're done with the texture so unlock it
	pTexture->UnlockRect(0);

	pTexture->Release();

ReleaseFrame:
	// Release the frame
	m_pNuiSensor->NuiImageStreamReleaseFrame(m_pDepthStreamHandle, &imageFrame);
}



void KinectDemo::init()
{
	CreateFirstConnected();
}

void KinectDemo::start()
{
	
}

GLuint displayList = 0;

void KinectDemo::draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix)
{
	glDisable(GL_CULL_FACE);

	glUseProgram(0);
	glEnable(GL_LIGHTING);
	glPointSize(1);
	glDisable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);

	glBegin(GL_QUADS);
	for (int i = 0; i < 640 * 480; i++)
	{
		if (depthMap[i] > 0.1 && depthMap[i] < 3 &&
			depthMap[i+1] > 0.1 && depthMap[i+1] < 3 &&
			depthMap[i+cDepthWidth] > 0.1 && depthMap[i+cDepthWidth] < 3 &&
			depthMap[i+1+cDepthWidth] > 0.1 && depthMap[i+1+cDepthWidth] < 3)
		{
			glm::vec3 a((i%cDepthWidth) / 320.0f,			2 - (i / cDepthWidth) / 320.0f,			depthMap[i]);
			glm::vec3 b(((i + 1) % cDepthWidth) / 320.0f,	2 - (i / cDepthWidth) / 320.0f,			depthMap[i + 1]);
			glm::vec3 c(((i + 1) % cDepthWidth) / 320.0f,	2 - ((i / cDepthWidth) + 1) / 320.0f,	depthMap[i + 1 + cDepthWidth]);
			glm::vec3 d((i%cDepthWidth) / 320.0f,			2 - ((i / cDepthWidth) + 1) / 320.0f,	depthMap[i + cDepthWidth]);

			glm::vec3 normal = glm::normalize(glm::cross(b - a, b - c));

			glNormal3fv(glm::value_ptr(normal));

			glVertex3fv(glm::value_ptr(a));
			glVertex3fv(glm::value_ptr(b));
			glVertex3fv(glm::value_ptr(c));
			glVertex3fv(glm::value_ptr(d));


		}
	}

	glEnd();


/*	if(data->hasData)
	{
		if(displayList == 0)
			displayList = glGenLists(1);
		glNewList(displayList, GL_COMPILE);
		glBegin(GL_TRIANGLES);
		float fac = 800;
		for(int x = 0; x < resx-1; x++)
		{
			for(int y = 0; y < resy-1; y++)
			{
				if(data->depths[x+resx*y] == 0 || data->depths[x+resx*y+1] == 0 || data->depths[x+resx*y+resx] == 0 || data->depths[x+resx*y+resx+1] == 0 || 
					data->depths[x+resx*y] == 65535 || data->depths[x+resx*y+1] == 65535 || data->depths[x+resx*y+resx] == 65535 || data->depths[x+resx*y+resx+1] == 65535)
					continue;


				glm::vec3 v1(1/64.0f,0,data->depths[x+resx*y] / fac - data->depths[x+resx*y+1] / fac);
				glm::vec3 v2(0,1/72.0f,data->depths[x+resx*y] / fac - data->depths[x+resx*y+resx] / fac);

				glm::vec3 normal = gmtl::makeCross(v1, v2);
				gmtl::normalize(normal);

				glNormal3f(normal[0], normal[1], normal[2]);

				//glColor3f(data->colors[3*(x+resx*y)+0] / 255.0f, data->colors[3*(x+resx*y)+1] / 255.0f, data->colors[3*(x+resx*y)+2] / 255.0f);
				glVertex3f(-5+(x)/64.0f,	-5+(resy-y)/72.0f,			-data->depths[x+resx*y] / fac);
				glVertex3f(-5+(x+1)/64.0f,	-5+(resy-y)/72.0f,			-data->depths[x+resx*y+1] / fac);
				glVertex3f(-5+(x+1)/64.0f,	-5+(resy-y-1)/72.0f,		-data->depths[x+resx*y+1+resx] / fac);

				glVertex3f(-5+(x+1)/64.0f,	-5+(resy-y-1)/72.0f,		-data->depths[x+resx*y+1+resx] / fac);
				glVertex3f(-5+(x)/64.0f,	-5+(resy-y-1)/72.0f,		-data->depths[x+resx*y+resx] / fac);
				glVertex3f(-5+(x)/64.0f,	-5+(resy-y)/72.0f,			-data->depths[x+resx*y] / fac);
			}
		}
		glEnd();
		glEndList();
		data->hasData = false;
	}
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	if(displayList != 0)
		glCallList(displayList);*/
}

void KinectDemo::update()
{

	if (NULL == m_pNuiSensor)
	{
		return;
	}

	if (WAIT_OBJECT_0 == WaitForSingleObject(m_hNextDepthFrameEvent, 0))
	{
		logger << "Got depth" << Log::newline;
		ProcessDepth();
	}

}


#endif