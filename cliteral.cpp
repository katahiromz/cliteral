// cliteral.cpp --- The C literal checker
// License: MIT
//////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <tchar.h>
#include <strsafe.h>
#include "CSTR_dump.h"
#include "CSTR_parse.h"
#include "mstr.h"
#include "resource.h"

HINSTANCE g_hInst = nullptr;
HWND g_hMainWnd = nullptr;

INT g_nUpdatingCStrUI = 0;

LPTSTR LoadStringDx(INT nID)
{
    static TCHAR s_asz[3][MAX_PATH];
    static INT s_i = 0;
    INT i = s_i++;
    s_i %= _countof(s_asz);
    s_asz[i][0] = 0;
    LoadString(g_hInst, nID, s_asz[i], _countof(s_asz[i]));
    return s_asz[i];
}

INT GetDlgItemString(HWND hwnd, INT nID, khmz::tstring& str)
{
    HWND hwndItem = GetDlgItem(hwnd, nID);
    INT cch = GetWindowTextLength(hwndItem);
    str.resize(cch);
    return GetWindowText(hwndItem, &str[0], cch + 1);
}

bool ConvertTrigraph(khmz::tstring& str)
{
    bool ret = false;
    ret |= mstr_replace_all(str, TEXT("?\?<"), TEXT("{"));
    ret |= mstr_replace_all(str, TEXT("?\?>"), TEXT("}"));
    ret |= mstr_replace_all(str, TEXT("?\?("), TEXT("["));
    ret |= mstr_replace_all(str, TEXT("?\?)"), TEXT("]"));
    ret |= mstr_replace_all(str, TEXT("?\?="), TEXT("#"));
    ret |= mstr_replace_all(str, TEXT("?\?/"), TEXT("\\"));
    ret |= mstr_replace_all(str, TEXT("?\?'"), TEXT("^"));
    ret |= mstr_replace_all(str, TEXT("?\?!"), TEXT("|"));
    ret |= mstr_replace_all(str, TEXT("?\?-"), TEXT("~"));
    return ret;
}

INT CALLBACK
EditWordBreakProc(LPTSTR lpch, INT ichCurrent, INT cch, INT code)
{
    return 0;
}

 
BOOL CStr_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    g_hMainWnd = hwnd;

    // Auto
    CheckDlgButton(hwnd, chx1, BST_CHECKED);
    EnableWindow(GetDlgItem(hwnd, psh1), FALSE);
    EnableWindow(GetDlgItem(hwnd, psh2), FALSE);

    // No conversion
    CheckRadioButton(hwnd, rad1, rad5, rad5);

    // No limit
    SendDlgItemMessage(hwnd, edt1, EM_LIMITTEXT, 0, 0);
    SendDlgItemMessage(hwnd, edt2, EM_LIMITTEXT, 0, 0);
    SendDlgItemMessage(hwnd, edt3, EM_LIMITTEXT, 0, 0);

    // Don't word-wrap
    SendDlgItemMessage(hwnd, edt1, EM_SETWORDBREAKPROC, 0, (LPARAM)EditWordBreakProc);
    SendDlgItemMessage(hwnd, edt2, EM_SETWORDBREAKPROC, 0, (LPARAM)EditWordBreakProc);
    SendDlgItemMessage(hwnd, edt3, EM_SETWORDBREAKPROC, 0, (LPARAM)EditWordBreakProc);

    return TRUE;
}

void CStr_SyncFromLeft(HWND hwnd)
{
    if (!g_nUpdatingCStrUI)
    {
        ++g_nUpdatingCStrUI;
        {
            SetDlgItemText(hwnd, edt2, TEXT(""));
            SetDlgItemText(hwnd, edt3, TEXT(""));

            khmz::tstring input;
            GetDlgItemString(hwnd, edt1, input);

            mstr_replace_all(input, TEXT("\r\n"), TEXT("\n"));

            int flags = 0;
            if (IsDlgButtonChecked(hwnd, rad1)) // Octal
                flags = khmz::CSTR_DUMP_ESCAPE_OCTAL;
            else if (IsDlgButtonChecked(hwnd, rad2)) // \xXX
                flags = 0;
            else if (IsDlgButtonChecked(hwnd, rad3)) // \uXXXX
                flags = khmz::CSTR_DUMP_ESCAPE_U16;
            else if (IsDlgButtonChecked(hwnd, rad4)) // \UXXXXXXXX
                flags = khmz::CSTR_DUMP_ESCAPE_U32;
            else // No conversion
                flags = khmz::CSTR_DUMP_ESCAPE_NOCONV;

            khmz::tstring output;
            khmz::CSTR_dump_text(output, input, flags);

            SetDlgItemText(hwnd, edt2, output.data());
        }
        --g_nUpdatingCStrUI;
    }
}

void get_binary_text(khmz::tstring& text, const void *ptr, size_t size)
{
    auto pb = (const uint8_t *)ptr;
    TCHAR buf[3];
    while (size-- > 0)
    {
        if (text.size())
            text += L' ';
        StringCchPrintf(buf, _countof(buf), TEXT("%02X"), *pb++);
        text += buf;
    }
}

void CStr_SyncFromRight(HWND hwnd)
{
    if (!g_nUpdatingCStrUI)
    {
        ++g_nUpdatingCStrUI;
        {
            SetDlgItemText(hwnd, edt1, TEXT(""));
            SetDlgItemText(hwnd, edt3, TEXT(""));

            khmz::tstring input;
            GetDlgItemString(hwnd, edt2, input);

            bool trigraph_found = ConvertTrigraph(input);

            int flags = 0;
            khmz::tstring output;
            khmz::CSTR_parse_text(output, input.data(), input.size(), flags);

            khmz::tstring bin_text;
            size_t byte_size = 0;
            size_t text_length = 0;
            switch ((flags & khmz::CSTR_PARSE_PREFIX_ALL))
            {
            case khmz::CSTR_PARSE_PREFIX_A:
                {
                    if (UTFIO_CODEPAGE == CP_UTF8)
                    {
                        std::string input2;
                        bool ret = khmz::u8_from_text(input2, input.data(), input.size());
                        if (!ret)
                        {
                            flags |= khmz::CSTR_PARSE_INVALID_CHARSET;
                        }
                        std::string output2;
                        khmz::CSTR_parse_u8(output2, input2.data(), input2.size(), flags);
                        text_length = output2.size();
                        byte_size = (output2.size() + 1) * sizeof(char);
                        get_binary_text(bin_text, output2.data(), byte_size);
                    }
                    else
                    {
                        std::string input2;
                        bool ret = khmz::ansi_from_text(input2, input.data(), input.size());
                        if (!ret)
                        {
                            flags |= khmz::CSTR_PARSE_INVALID_CHARSET;
                        }
                        std::string output2;
                        khmz::CSTR_parse_ansi(output2, input2.data(), input2.size(), flags);
                        text_length = output2.size();
                        byte_size = (output2.size() + 1) * sizeof(char);
                        get_binary_text(bin_text, output2.data(), byte_size);
                    }
                }
                break;
            case khmz::CSTR_PARSE_PREFIX_L:
            case khmz::CSTR_PARSE_PREFIX_U16:
                {
                    std::u16string input2;
                    khmz::u16_from_text(input2, input.data(), input.size());
                    std::u16string output2;
                    khmz::CSTR_parse_u16(output2, input2.data(), input2.size(), flags);
                    text_length = output2.size();
                    byte_size = (output2.size() + 1) * sizeof(char16_t);
                    get_binary_text(bin_text, output2.data(), byte_size);
                }
                break;
            case khmz::CSTR_PARSE_PREFIX_U8:
                {
                    std::string input2;
                    khmz::u8_from_text(input2, input.data(), input.size());
                    std::string output2;
                    khmz::CSTR_parse_u8(output2, input2.data(), input2.size(), flags);
                    text_length = output2.size();
                    byte_size = (output2.size() + 1) * sizeof(char);
                    get_binary_text(bin_text, output2.data(), byte_size);
                }
                break;
            case khmz::CSTR_PARSE_PREFIX_U32:
                {
                    std::u32string input2;
                    khmz::u32_from_text(input2, input.data(), input.size());
                    std::u32string output2;
                    khmz::CSTR_parse_u32(output2, input2.data(), input2.size(), flags);
                    text_length = output2.size();
                    byte_size = (output2.size() + 1) * sizeof(char32_t);
                    get_binary_text(bin_text, output2.data(), byte_size);
                }
                break;
            }

            mstr_replace_all(output, TEXT("\r\n"), TEXT("\n"));
            mstr_replace_all(output, TEXT("\n"), TEXT("\r\n"));

            if (flags & khmz::CSTR_PARSE_ERRORS)
            {
                SetDlgItemText(hwnd, edt1, TEXT(""));
            }
            else
            {
                SetDlgItemText(hwnd, edt1, output.data());
            }

            khmz::tstring info;
            if (flags & khmz::CSTR_PARSE_SYNTAX_ERROR)
                info += LoadStringDx(IDS_SYNTAX_ERROR);
            else if (flags & khmz::CSTR_PARSE_INVALID_ESCAPE)
                info += LoadStringDx(IDS_INVALID_ESCAPE);
            else if (flags & khmz::CSTR_PARSE_QUOTE_NOT_FOUND)
                info += LoadStringDx(IDS_QUOTE_NOT_FOUND);
            else if (flags & khmz::CSTR_PARSE_QUOTE_NOT_CLOSED)
                info += LoadStringDx(IDS_QUOTE_NOT_CLOSED);
            else if (flags & khmz::CSTR_PARSE_PREFIX_MISMATCH)
                info += LoadStringDx(IDS_PREFIX_MISMATCH);
            else if (flags & khmz::CSTR_PARSE_NOT_QUOTED)
                info += LoadStringDx(IDS_NOT_QUOTED);
            else if (flags & khmz::CSTR_PARSE_INVALID_CHARSET)
                info += LoadStringDx(IDS_INVALID_CHARSET);
            else if (flags & khmz::CSTR_PARSE_DELIMITER_TOO_LONG)
                info += LoadStringDx(IDS_DELIMITER_TOO_LONG);
            else if (flags & khmz::CSTR_PARSE_RAW_LITERAL_NOT_CLOSED)
                info += LoadStringDx(IDS_RAW_LITERAL_NOT_CLOSED);

            if (info.empty())
            {
                if (flags & khmz::CSTR_PARSE_PREFIX_A)
                    info += LoadStringDx(IDS_ANSI_STRING);
                else if (flags & khmz::CSTR_PARSE_PREFIX_L)
                    info += LoadStringDx(IDS_WIDE_STRING);
                else if (flags & khmz::CSTR_PARSE_PREFIX_U8)
                    info += LoadStringDx(IDS_UTF8_STRING);
                else if (flags & khmz::CSTR_PARSE_PREFIX_U16)
                    info += LoadStringDx(IDS_UTF16_STRING);
                else if (flags & khmz::CSTR_PARSE_PREFIX_U32)
                    info += LoadStringDx(IDS_UTF32_STRING);
                info += TEXT("\r\n");
                info += LoadStringDx(IDS_TEXT_LENGTH);
                info += khmz::to_tstring(text_length);
                info += TEXT("\r\n");
                info += LoadStringDx(IDS_BINARY_SIZE);
                info += khmz::to_tstring(byte_size);
                info += TEXT(' ');
                info += LoadStringDx(IDS_BYTES);
                info += TEXT("\r\n");
                info += LoadStringDx(IDS_BYTE_SEQUENCE);
                info += bin_text;
            }

            info += TEXT("\r\n");

            if (trigraph_found)
            {
                info += LoadStringDx(IDS_TRIGRAPH_FOUND);
                info += TEXT("\r\n");
            }

            SetDlgItemText(hwnd, edt3, (TCHAR*)info.data());
        }
        --g_nUpdatingCStrUI;
    }
}

void OnExamples(HWND hwnd)
{
    HMENU hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_EXAMPLES_MENU));
    HMENU hSubMenu = GetSubMenu(hMenu, 0);

    SetForegroundWindow(hwnd);

    RECT rc;
    GetWindowRect(GetDlgItem(hwnd, psh4), &rc);

    POINT pt = { rc.left, (rc.top + rc.bottom) / 2 };

    TPMPARAMS params = { sizeof(params), rc };
    constexpr auto flags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL | TPM_RETURNCMD;
    const INT id = TrackPopupMenuEx(hSubMenu, flags, pt.x, pt.y, hwnd, &params);
    if (id != 0 && id != -1)
    {
        MENUITEMINFO info = { sizeof(info), MIIM_STRING };
        TCHAR szText[MAX_PATH];
        info.dwTypeData = szText;
        info.cch = _countof(szText);
        GetMenuItemInfo(hSubMenu, id, FALSE, &info);

        khmz::tstring text = szText;
        mstr_replace_all(text, TEXT("&&"), TEXT("&"));

        ++g_nUpdatingCStrUI;
        SetDlgItemText(hwnd, edt2, text.c_str());
        --g_nUpdatingCStrUI;

        CStr_SyncFromRight(hwnd);
    }

    DestroyMenu(hMenu);
}

void CStr_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case edt1: // Left
        if (codeNotify != EN_CHANGE)
            break;
        if (IsDlgButtonChecked(hwnd, chx1)) // Auto?
        {
            CStr_SyncFromLeft(hwnd);
        }
        break;
    case edt2: // Right
        if (codeNotify != EN_CHANGE)
            break;
        if (IsDlgButtonChecked(hwnd, chx1)) // Auto?
        {
            CStr_SyncFromRight(hwnd);
        }
        break;
    case psh1: // >>
        CStr_SyncFromLeft(hwnd);
        break;
    case psh2: // <<
        CStr_SyncFromRight(hwnd);
        break;
    case psh3: // Manual
        ::ShellExecute(hwnd, nullptr, LoadStringDx(IDS_MANUAL_URL), nullptr, nullptr, SW_SHOWNORMAL);
        break;
    case psh4: // Examples
        OnExamples(hwnd);
        break;
    case psh5: // Calc
        ::ShellExecute(hwnd, nullptr, TEXT("calc.exe"), nullptr, nullptr, SW_SHOWNORMAL);
        break;
    case chx1: // Auto
        if (codeNotify != BN_CLICKED)
            break;
        if (IsDlgButtonChecked(hwnd, chx1)) // Auto?
        {
            EnableWindow(GetDlgItem(hwnd, psh1), FALSE);
            EnableWindow(GetDlgItem(hwnd, psh2), FALSE);
        }
        else
        {
            EnableWindow(GetDlgItem(hwnd, psh1), TRUE);
            EnableWindow(GetDlgItem(hwnd, psh2), TRUE);
        }
        break;
    case rad1:
    case rad2:
    case rad3:
    case rad4:
    case rad5:
        if (codeNotify != BN_CLICKED)
            break;
        CStr_SyncFromLeft(hwnd);
        break;
    }
}

LRESULT CStr_OnNotify(HWND hwnd, int idFrom, LPNMHDR pnmhdr)
{
    switch (pnmhdr->code)
    {
    case PSN_APPLY: // Apply
        return SetDlgMsgResult(hwnd, WM_NOTIFY, PSNRET_NOERROR);
    }
    return 0;
}

void CStr_OnDestroy(HWND hwnd)
{
    g_hMainWnd = nullptr;
    PostQuitMessage(0);
}

INT_PTR CALLBACK
CStr_DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, CStr_OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, CStr_OnCommand);
        HANDLE_MSG(hwnd, WM_NOTIFY, CStr_OnNotify);
        HANDLE_MSG(hwnd, WM_DESTROY, CStr_OnDestroy);
    }
    return 0;
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    g_hInst = hInstance;

    InitCommonControls();

    PROPSHEETPAGE psp = { sizeof(psp) };
    HPROPSHEETPAGE hpsp[1];
    UINT iPage = 0;
    UINT nStartPage = 0;

    psp.pszTemplate = MAKEINTRESOURCE(IDD_CSTRLITERAL);
    psp.pfnDlgProc = CStr_DlgProc;
    psp.dwFlags = PSP_DEFAULT;
    psp.hInstance = g_hInst;
    hpsp[iPage++] = ::CreatePropertySheetPage(&psp);

    assert(iPage <= _countof(hpsp));
    assert(nStartPage < _countof(hpsp));

    PROPSHEETHEADER psh = { sizeof(psh) };
    psh.dwFlags = PSH_USEHICON | PSH_NOAPPLYNOW;
    psh.hInstance = g_hInst;
    psh.hwndParent = nullptr;
    psh.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MAINICON));
    psh.nPages = iPage;
    psh.phpage = hpsp;
    psh.pszCaption = LoadStringDx(IDS_TITLE);
    psh.nStartPage = nStartPage;

    ::PropertySheet(&psh);

    return 0;
}
