/*++
Copyright (c) Microsoft Corporation

Module Name:
- clipboard.hpp

Abstract:
- This module is used for clipboard operations

Author(s):
- Michael Niksa (MiNiksa) 10-Apr-2014
- Paul Campbell (PaulCam) 10-Apr-2014

Revision History:
- From components of clipbrd.h/.c
--*/

#pragma once

#include "precomp.h"

#include "..\..\host\screenInfo.hpp"

namespace Microsoft::Console::Interactivity::Win32
{
    class Clipboard
    {
    public:
        static Clipboard& Instance();

        void Copy();
        void StringPaste(_In_reads_(cchData) PCWCHAR pwchData,
                            const size_t cchData);
        void Paste();
    private:
        std::deque<std::unique_ptr<IInputEvent>> TextToKeyEvents(_In_reads_(cchData) const wchar_t* const pData,
                                                                    const size_t cchData);

        void StoreSelectionToClipboard();

        [[nodiscard]]
        NTSTATUS RetrieveTextFromBuffer(const SCREEN_INFORMATION& screenInfo,
                                        const bool fLineSelection,
                                        const UINT cRectsSelected,
                                        _In_reads_(cRectsSelected) const SMALL_RECT* const rgsrSelection,
                                        _Out_writes_(cRectsSelected) PWCHAR* const rgpwszTempText,
                                        _Out_writes_(cRectsSelected) size_t* const rgTempTextLengths);

        [[nodiscard]]
        NTSTATUS CopyTextToSystemClipboard(const UINT cTotalRows,
                                           _In_reads_(cTotalRows) const PWCHAR* const rgTempRows,
                                           _In_reads_(cTotalRows) const size_t* const rgTempRowLengths);

        bool FilterCharacterOnPaste(_Inout_ WCHAR * const pwch);

#ifdef UNIT_TESTING
        friend class ClipboardTests;
#endif
    };
}
