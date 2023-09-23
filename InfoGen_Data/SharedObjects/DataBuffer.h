#pragma once

#ifndef _PARAM_BUFFER_
#define _PARAM_BUFFER_

#if defined(_EXPORT) && !defined(EXTERNAL_CALL)
#define EXTERNAL_CALL __declspec(dllexport)
#else
#define EXTERNAL_CALL __declspec(dllimport)
#endif

#endif