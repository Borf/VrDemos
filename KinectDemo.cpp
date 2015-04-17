#ifndef NOKINECT
#include "KinectDemo.h"

#include <windows.h>
#include <gl/GL.h>
#include <gmtl/Vec.h>
#include <gmtl/VecOps.h>
#include <gmtl/Generate.h>
#include <cavelib/Components/Panel.h>
#include <cavelib/components/Button.h>
#include <cavelib/LayoutManagers/TableLayout.h>

int resx = XN_VGA_X_RES;
int resy = XN_VGA_Y_RES;

KinectDemo::KinectDemo(void) : Demo("Kinect")
{
}


KinectDemo::~KinectDemo(void)
{
}

xn::Context context;
xn::DepthGenerator depth;
xn::ImageGenerator imageGenerator;

void KinectDemo::init()
{
	vpr::GUID new_guid("d6be4359-e8cf-41fc-a72b-a5b4f3f29aa2");
	data.init(new_guid);
	data->doCopy = false;
	data->hasData = false;

	kinectLoaded = false;
}

void KinectDemo::start()
{
	if(data.isLocal())
	{
		if(!kinectLoaded)
		{
			kinectLoaded = true;
			XnStatus nRetVal = XN_STATUS_OK;

			nRetVal = context.Init();
			nRetVal = depth.Create(context);
			imageGenerator.Create(context);

			XnMapOutputMode mapMode;
			mapMode.nXRes = resx;
			mapMode.nYRes = resy;
			mapMode.nFPS = 30;
			nRetVal = depth.SetMapOutputMode(mapMode);
			nRetVal = imageGenerator.SetMapOutputMode(mapMode);

			nRetVal = context.StartGeneratingAll();
		}


		XnStatus nRetVal = context.WaitAndUpdateAll();
		if (nRetVal != XN_STATUS_OK)
		{
			printf("Failed updating data: %s\n", xnGetStatusString(nRetVal));
			//	continue;
		}

		// Take current depth map
		const XnDepthPixel* pDepthMap = depth.GetDepthMap();
		const XnUInt8* pImageMap = imageGenerator.GetImageMap();

		memcpy(data->depths, pDepthMap, sizeof(XnDepthPixel) * resx * resy);
		memcpy(data->colors, pImageMap, sizeof(XnUInt8) * resx * resy * 3);
		data->doCopy = true;
		data->hasData = true;
	}
}

GLuint displayList = 0;

void KinectDemo::draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix)
{
	glLoadIdentity();
	glDisable(GL_CULL_FACE);


	if(data->hasData)
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
		glCallList(displayList);
}

void KinectDemo::update()
{
	if(data.isLocal())
	{
		XnStatus nRetVal = context.WaitNoneUpdateAll();
		if (nRetVal != XN_STATUS_OK)
		{
			printf("Failed updating data: %s\n", xnGetStatusString(nRetVal));
			//	continue;
		}
	
	/*	if(!data->hasData)
		{
			// Take current depth map
			const XnDepthPixel* pDepthMap = depth.GetDepthMap();
			const XnUInt8* pImageMap = imageGenerator.GetImageMap();

			memcpy(data->depths, pDepthMap, sizeof(XnDepthPixel) * resx * resy);
			memcpy(data->colors, pImageMap, sizeof(XnUInt8) * resx * resy * 3);
			data->doCopy = true;
			data->hasData = true;
		}*/
	}

}

class KinectDemoPanel : public Panel
{
	KinectDemo* demo;

	class KinectButton : public Button
	{
		KinectDemo* demo;
	public:
		KinectButton(KinectDemo* demo) : Button("Snapshot")
		{
			this->demo = demo;
		}
	};

public:
	KinectDemoPanel(KinectDemo* demo) : Panel(new TableLayoutManager(1))
	{
		this->demo = demo;
		add(new KinectButton(demo));
	}

	virtual float minWidth() 
	{
		return 1.8;
	}

	virtual float minHeight() 
	{
		return 2;
	}

};

Panel* KinectDemo::getPanel()
{
	return new KinectDemoPanel(this);
}
#endif