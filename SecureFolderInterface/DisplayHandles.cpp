#include "DisplayHandles.h"

int paint(HWND hwnd, HDC hdc, PAINTSTRUCT ps)
{
    static HBRUSH inputBrush = CreateSolidBrush(RGB_WHITE);
    static HBRUSH edgeInputBrush = CreateSolidBrush(RGB_BLACK);
    static HFONT italicFont = CreateFont(0, 0, 0, 0, FW_LIGHT, true, false, false, ANSI_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, NULL);
    static HFONT defaultFont = CreateFont(0, 0, 0, 0, FW_NORMAL, false, false, false, ANSI_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, NULL);

    SelectObject(hdc, defaultFont);

    RECT fillRect;

    //PATH OF THE TARGET FILE
    fillRect.left = POS_TARGET_PATH_X;
    fillRect.top = POS_TARGET_PATH_Y;
    fillRect.bottom = POS_TARGET_PATH_Y + DIM_TARGET_PATH_HEIGHT;
    fillRect.right = POS_TARGET_PATH_X + DIM_TARGET_PATH_WIDTH;

    FillRect(hdc, &fillRect, inputBrush);
    FrameRect(hdc, &fillRect, edgeInputBrush);

    fillRect.left += 5;
    fillRect.right -= 5;

    if (fields.targetPath[0] == '\0')
    {
        HFONT lastFont = (HFONT) SelectObject(hdc, italicFont);
        SetTextColor(hdc, RGB_GREY);
        DrawTextA(hdc, "Put the target's path here", -1, &fillRect, DT_LEFT | DT_VCENTER  | DT_SINGLELINE | DT_INTERNAL);
        SelectObject(hdc, lastFont);
        SetTextColor(hdc, RGB_BLACK);
    }
    else
    {
        SetTextColor(hdc, RGB_DARK_GREY);
        DrawTextA(hdc, fields.targetPath, -1, &fillRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_INTERNAL);
        SetTextColor(hdc, RGB_BLACK);
    }

    //PATH REASEARCH BUTTON
    fillRect.left = POS_RESEARCH_BTN_X;
    fillRect.top = POS_RESEARCH_BTN_Y;
    fillRect.bottom = POS_RESEARCH_BTN_Y + DIM_RESEARCH_BTN_HEIGHT;
    fillRect.right = POS_RESEARCH_BTN_X + DIM_RESEARCH_BTN_WIDTH;

    FillRect(hdc, &fillRect, inputBrush);
    DrawTextA(hdc, "...", -1, &fillRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_INTERNAL);
    FrameRect(hdc, &fillRect, edgeInputBrush);

    //CYPHER / DECYPHER BUTTON

    fillRect.left = POS_CYPHER_BTN_X;
    fillRect.top = POS_CYPHER_BTN_Y;
    fillRect.bottom = POS_CYPHER_BTN_Y + DIM_CYPHER_BTN_HEIGHT;
    fillRect.right = POS_CYPHER_BTN_X + DIM_CYPHER_BTN_WIDTH;

    FillRect(hdc, &fillRect, inputBrush);
    FrameRect(hdc, &fillRect, edgeInputBrush);

    fillRect.left += 5;
    fillRect.right -= 5;

    switch (fields.currentMode)
    {
    case MODE::MODE_UNKNOWN: 
        DrawTextA(hdc, "-", -1, &fillRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_INTERNAL);
        break;
    case MODE::MODE_CYPHER:
        DrawTextA(hdc, "CYPHER", -1, &fillRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_INTERNAL);
        break;
    case MODE::MODE_DECYPHER:
        DrawTextA(hdc, "DECYPHER", -1, &fillRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_INTERNAL);
        break;
    default:
        break;
    }

    //OPTIONS

    fillRect.left = POS_OPTIONS_RECT_X;
    fillRect.top = POS_OPTIONS_RECT_Y;
    fillRect.bottom = POS_OPTIONS_RECT_Y + DIM_OPTIONS_RECT_HEIGHT;
    fillRect.right = POS_OPTIONS_RECT_X + DIM_OPTIONS_RECT_WIDTH;

    FillRect(hdc, &fillRect, inputBrush);
    FrameRect(hdc, &fillRect, edgeInputBrush);

    

    //SELECTED BOX
    switch (fields.selected)
    {
    case SELECTED::TARGET_PATH :
        fillRect.top = POS_TARGET_PATH_Y + 1;
        fillRect.left = POS_TARGET_PATH_X + 1;
        fillRect.bottom = POS_TARGET_PATH_Y + DIM_TARGET_PATH_HEIGHT - 1;
        fillRect.right = POS_TARGET_PATH_X + DIM_TARGET_PATH_WIDTH - 1;
        DrawFocusRect(hdc, &fillRect);
        break;
    case SELECTED::CYPHER_BTN :
        fillRect.top = POS_CYPHER_BTN_Y + 1;
        fillRect.left = POS_CYPHER_BTN_X + 1;
        fillRect.bottom = POS_CYPHER_BTN_Y + DIM_CYPHER_BTN_HEIGHT - 1;
        fillRect.right = POS_CYPHER_BTN_X + DIM_CYPHER_BTN_WIDTH - 1;
        DrawFocusRect(hdc, &fillRect);
        break;
    case SELECTED::OPTIONS :
        //TODO: finish options
        break;
    default:
        break;
    }

    return 0;
}