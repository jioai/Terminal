/********************************************************
*                                                       *
*   Copyright (C) Microsoft. All rights reserved.       *
*                                                       *
********************************************************/

#include "precomp.h"

#include "DispatchCommon.hpp"
#include "../../types/inc/Viewport.hpp"

using namespace Microsoft::Console::Types;
using namespace Microsoft::Console::VirtualTerminal;

// Method Description:
// - Resizes the window to the specified dimensions, in characters.
// Arguments:
// - pConApi: The ConGetSet implementation to call back into.
// - usWidth: The new width of the window, in columns
// - usHeight: The new height of the window, in rows
// Return Value:
// True if handled successfully. False othewise.
bool DispatchCommon::s_ResizeWindow(_Inout_ ConGetSet* const pConApi,
                                    const unsigned short usWidth,
                                    const unsigned short usHeight)
{
    SHORT sColumns = 0;
    SHORT sRows = 0;

    // We should do nothing if 0 is passed in for a size.
    bool fSuccess = SUCCEEDED(UShortToShort(usWidth, &sColumns)) &&
                    SUCCEEDED(UIntToShort(usHeight, &sRows)) && 
                    (usWidth > 0 && usHeight > 0);
    
    if (fSuccess)
    {
        CONSOLE_SCREEN_BUFFER_INFOEX csbiex = { 0 };
        csbiex.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
        fSuccess = !!pConApi->GetConsoleScreenBufferInfoEx(&csbiex);

        if (fSuccess)
        {
            const Viewport oldViewport = Viewport::FromInclusive(csbiex.srWindow);
            const Viewport newViewport = Viewport::FromDimensions(oldViewport.Origin(),
                                                                  sColumns,
                                                                  sRows);
            // Always resize the width of the console
            csbiex.dwSize.X = sColumns;
            // Only set the screen buffer's height if it's currently less than 
            //  what we're requesting.
            if(sRows > csbiex.dwSize.Y)
            {
                csbiex.dwSize.Y = sRows;
            }

            // SetConsoleWindowInfo expect inclusive rects
            SMALL_RECT sri = newViewport.ToInclusive();

            // SetConsoleScreenBufferInfoEx however expects exclusive rects
            SMALL_RECT sre = newViewport.ToExclusive();
            csbiex.srWindow = sre;

            fSuccess = !!pConApi->SetConsoleScreenBufferInfoEx(&csbiex);
            if (fSuccess)
            {
                fSuccess = !!pConApi->SetConsoleWindowInfo(true, &sri);
            }
        }   
    }
    return fSuccess;
}

// Routine Description:
// - Force the host to repaint the screen.
// Arguments:
// - pConApi: The ConGetSet implementation to call back into.
// Return Value:
// True if handled successfully. False othewise.
bool DispatchCommon::s_RefreshWindow(_Inout_ ConGetSet* const pConApi)
{
    return !!pConApi->PrivateRefreshWindow();
}

// Routine Description:
// - Force the host to tell the renderer to not emit anything in response to the 
//      next resize event. This is used by VT I/O to prevent a terminal from 
//      requesting a resize, then having the renderer echo that to the terminal,
//      then having the terminal echo back to the host...
// Arguments:
// - pConApi: The ConGetSet implementation to call back into.
// Return Value:
// True if handled successfully. False othewise.
bool DispatchCommon::s_SuppressResizeRepaint(_Inout_ ConGetSet* const pConApi)
{
    return !!pConApi->PrivateSuppressResizeRepaint();
}
