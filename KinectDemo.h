#pragma once
#ifndef NOKINECT
#include "demo.h"
#include <XnCppWrapper.h>
#include <vpr/IO/SerializableObject.h>
#include <vpr/IO/ObjectReader.h>
#include <vpr/IO/ObjectWriter.h>
#include <plugins/ApplicationDataManager/UserData.h>


class KinectData : public vpr::SerializableObject
{
public:
	bool doCopy;
	bool hasData;
	XnDepthPixel depths[640*480];
	XnUInt8 colors[640*480*3];
	void writeObject(vpr::ObjectWriter* writer)
	{
		writer->writeBool(doCopy);
		if(doCopy)
		{
			for(int i = 0; i < 640*480; i++)
				writer->writeUint16(depths[i]);
			for(int i = 0; i < 640*480*3; i++)
				writer->writeUint8(colors[i]);
		}
		doCopy = false;
	}
	void readObject(vpr::ObjectReader* reader)
	{
		if(reader->readBool())
		{
			for(int i = 0; i < 640*480; i++)
				depths[i] = reader->readUint16();
			for(int i = 0; i < 640*480*3; i++)
				colors[i] = reader->readUint8();
			hasData = true;
		}
	}
};

class KinectDemo :	public Demo
{
	bool kinectLoaded;
public:
	cluster::UserData< KinectData > data;
	KinectDemo(void);
	~KinectDemo(void);

	virtual void init();
	virtual void start();
	virtual void draw(glm::mat4 projectionMatrix);
	virtual void update();
	virtual Panel* getPanel();

};

#endif
