/*************************************************************

     Generic C3D Class
	 by Alastair Macleod

	 http://www.mocap.ca/
	 al@mocap.ca

	 Based on code and documentation found on  www.c3d.org

	 All rights reserved.

	 Contact for licensing information.

**************************************************************/

/*  
   Sept 12, 2006:  Ripped out maya code and built as as a base class

 */


#ifndef ___C3DFILE_H_
#define ___C3DFILE_H_

#define PEEL_C3D_VERSION "1.0"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <locale>
#include <errno.h>

#ifndef _WIN32
#include <string.h>
#include <stdlib.h>
#endif

using namespace std;

namespace peel
{

struct c3dHeader {
	unsigned short  key;
	unsigned short	num_markers;   
	unsigned short	num_channels;  
	unsigned short	first_field;   
	unsigned short	last_field;    
	unsigned short	max_interpolation;	
	char	        scale_factor[4];   	
	unsigned short	body_data_offset;   
	unsigned short	frames_per_field;   
	char	        frame_rate[4];      
};

struct c3dTrajDataT {
	unsigned short x;
	unsigned short y;
	unsigned short z;
	unsigned short extra;
};

struct c3dMarker
{
	string name;
	void *userData;
};


struct c3dTimeCode
{
	bool used;
	bool dropFrame;
	std::vector<short> fieldNumbers;
	int  standard;
	int  subFramePerSample;
	int  offset;
	int  timecode[4];
};


//! Abstract and platform independant representation of a c3d file. 
/*! Subclass and override virtual methods to handle specific implementation.
 *  Features timecode, analog and point data, labels.  */

class c3dFile
{
public:
	c3dFile();
	virtual ~c3dFile();

	//! Open this file
	bool open(const char* fileName);

	//! Parse this file, call after open has returned true
	bool parse(const char* groupName, bool useTimecode, bool debug, bool fullFrames); 

	//! Read parameters
	/*! Sets members based on parameters in file and reads labels */
	bool readParam(size_t id, char* param_name);  

	//! Read the label data.
	/*! @param pointData true read points, false: read analog labels */
	bool readLabels(std::vector<string> &);  

	bool        m_validateLabels;  //!< if true, remove invalid chars from label names

	FILE*       m_file;         //!< The file being read

	short       m_analogFrames; //!< Number of analog frames
	short       m_pointFrames;  //!< Number of point frames

	c3dTimeCode m_timecode;     //!< Timecode start and rate, read from parameters
	double      m_timecode_offset; //!<< Calculated timecode offset in seconds

	c3dHeader   m_header;       //!< The raw header data
	float      m_scaleFactor;  //!< C3D Data Scale factor, converted from header
	float      m_scaleValue;   //!< Value scale factor, default is 1.0, for unit conversion
	float      m_frameRate;    //!< Sample rate, converted from header
	char        m_procType;     //!< Data format, decoded by convertToFloat
	bool        m_debug;        //!< Write debug data to cout during parsing 
	string      m_units;        //!< Units as specified by the UNITS parameter


	std::vector<std::string>  m_analogLabels;  //!< Names of analog labels
	std::vector<std::string>  m_pointLabels;   //!< Names of marker/point cloud labels

	std::vector<std::string>  m_subjectNames;
	std::vector<std::string>  m_subjectPrefixes;

	char* m_groups[255];  //!< Name of groups in parameter section 

	//! Override this metod to create the main group for the data.  
	/*! @param name provided by as parse(..., groupName, ...) */
	virtual bool createGroup(string name) = 0;   

	//! Create a point in the cloud.
	/*! @param name label as defined in c3d file */
	/*! @param isPoint true if its a point, false if analog data */
	virtual bool createLocator(size_t i, string name, bool isPoint) = 0;


	virtual void setSizes(size_t pointSize, size_t channelSize) = 0;

	//! Add a key to a position Locator
	/*! @param marker the Locator to add key to, in order that they were created
 	    @param sample time location to add key, may include timecode offset.  */	
	virtual bool addKey(size_t marker, double sample, float x, float y, float z) = 0;

	//! Add a key to a analog Locator
	/*! @param marker the Locator to add the key to, in order that they were created
            @param sample time value to add key to, may include timecode
	    @param value analog value for key */
	virtual bool addAnalogKey(size_t marker, double sample, float value) = 0;

	//! Set the frame rate
	virtual void setRate(double rate) = 0;

	//! Set the frame range for the importing file.  Called before parsing starts
	virtual void setRange(int start, int end) = 0;

	//! Called when a frame has completed.  Timecode is not included here?!
	virtual bool frameDone(int time) = 0;

	//! Call when parsing is complete
	virtual void allDone() = 0;

	virtual void debugMessage(const char *fmt, ...);

protected:
	//! Convert a float to a c3d format bytes
	void covertFloatTo(float f, char* bytes, unsigned char proc);

	//! Change endianness of byte where required
	void conform(unsigned short &byte, unsigned char proc);

	//! Convert bytes from c3d file in to a float type
	float convertToFloat(char bytes[4], unsigned char proc);


};

}// namespace
#endif
