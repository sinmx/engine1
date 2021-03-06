/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#ifndef __XPHR_H__
#define __XPHR_H__

#include <assert.h>

#ifdef _DEBUG
  void AxDebugBreak(HRESULT hr);
  #define AX_DEBUG_BREAK(hr)  AxDebugBreak(hr)
  #define AX_ASSERT assert
  #define AX_VERIFY(f) AX_ASSERT(f)
#else
  #define AX_DEBUG_BREAK(hr) 
  #define AX_ASSERT assert
  #define AX_VERIFY(f) f
#endif

#ifndef USES_HR
    #define USES_HR HRESULT hr = S_OK; int nCleanupHandlerLineNumber = 0;
    #define CLEANUP  PseudoExceptionCleanup: nCleanupHandlerLineNumber = __LINE__;
    #define E_JUMP() if( (0 == nCleanupHandlerLineNumber) || (__LINE__ < nCleanupHandlerLineNumber) ) {AX_DEBUG_BREAK(hr); goto PseudoExceptionCleanup;}
    #define E_W32(Win32ReturnCode) {if(ERROR_SUCCESS != Win32ReturnCode){hr = HRESULT_FROM_WIN32(Win32ReturnCode); E_JUMP();}}
    #define E_NULL(Expression) if(NULL == (Expression)) {E_W32(ERROR_INVALID_PARAMETER);}
    #define E_MEM(Expression) if(NULL == (Expression)) {E_W32(ERROR_NOT_ENOUGH_MEMORY);}
    #define E_HR(HresultExpression) {hr = HresultExpression; if(FAILED(hr)) {E_JUMP();}}
    #define E_LW32() {const DWORD dwError = GetLastError(); if(ERROR_SUCCESS != dwError){hr = HRESULT_FROM_WIN32(dwError); E_JUMP()}}
    #define E_BW32(BooleanWin32Call) if(!BooleanWin32Call){const DWORD dwError = GetLastError(); ASSERT(ERROR_SUCCESS != dwError); hr = HRESULT_FROM_WIN32(dwError); E_JUMP();}
    #define HRESULT_TO_WIN32(HresultExpression) ((HRESULT_FACILITY(HresultExpression) == FACILITY_WIN32) ? HRESULT_CODE(HresultExpression) : (HresultExpression))
#endif //USES_HR

#endif //__XPHR_H__
