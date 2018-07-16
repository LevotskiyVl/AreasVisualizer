#ifndef GIP_CONFIG_H_
#define GIP_CONFIG_H_

#ifndef DLLEXPORT
#if defined (_WIN32)    \
 || defined (__WIN32)   \
 || defined (__WIN32__) \
 || defined (WIN32)

    #define DLLEXPORT __declspec(dllexport)

#else
    #define DLLEXPORT
#endif


#endif//GIPDLLEXPORT

#endif//GIP_CONFIG_H_
