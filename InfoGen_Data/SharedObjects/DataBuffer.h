#pragma once

#ifndef _PARAM_BUFFER_
#define _PARAM_BUFFER_

#if defined(_EXPORT) && !defined(EXTERNAL_CALL)
#define EXTERNAL_CALL __declspec(dllexport)
#else
#define EXTERNAL_CALL __declspec(dllimport)
#endif

#ifdef _DEBUG
#define _DATABUFFER_PATH "SharedObjects.so"
#define _DATABUFFER_PATH_INNER "SharedObjects.so"
#else
#define _DATABUFFER_PATH "./InfoGen_Data/SharedObjects/DataBuffer.so"
#define _DATABUFFER_PATH_INNER "./SharedObjects/DataBuffer.so"
#endif

enum OutputFormat
{
	HTML,
	MD
};

#endif