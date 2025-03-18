#include "ClockWidget.h"

// Used for drawing segments which have not changed and initial set-up
void ClockWidget::drawSegment(int seg, uint32_t color) {
    m_manager.spr_fillRoundRect(segLoc[seg][0], segLoc[seg][1], segLoc[seg][2], segLoc[seg][3], 8, color);
}

void ClockWidget::drawDisappearBT(int seg, int seq) {
    int x;
    int y;
    int l;
    int w;
    x = segLoc[seg][0];
    y = segLoc[seg][1];
    l = segLoc[seg][2];
    if (seg == 0 || seg == 3 || seg == 6) {
    } else {
        w = (20 * (4 - seq));
        if (seq == 4)
            l = 0;
    }
    m_manager.spr_fillRoundRect(x, y, l, w, 8, m_fgColor);
}

void ClockWidget::drawDisappearTB(int seg, int seq) {
    int x;
    int y;
    int l;
    int w;
    x = segLoc[seg][0];
    l = segLoc[seg][2];
    if (seg == 0 || seg == 3 || seg == 6) {
    } else {
        y = segLoc[seg][1] + (20 * (seq));
        w = (20 * (4 - seq));
        if (seq == 4)
            l = 0;
    }
    m_manager.spr_fillRoundRect(x, y, l, w, 8, m_fgColor);
}

void ClockWidget::drawAppearTB(int seg, int seq) {
    int x;
    int y;
    int l;
    int w;
    x = segLoc[seg][0];
    y = segLoc[seg][1];
    l = segLoc[seg][2];
    if (seg == 0 || seg == 3 || seg == 6) {
    } else {
        w = (20 * (seq));
        if (seq == 0)
            l = 0;
    }
    m_manager.spr_fillRoundRect(x, y, l, w, 8, m_fgColor);
}

void ClockWidget::drawAppearBT(int seg, int seq) {
    int x;
    int y;
    int l;
    int w;
    x = segLoc[seg][0];
    l = segLoc[seg][2];
    if (seg == 0 || seg == 3 || seg == 6) {
    } else {
        y = segLoc[seg][1] + (20 * (4 - seq));
        w = (20 * (seq));
        if (seq == 0)
            l = 0;
    }
    m_manager.spr_fillRoundRect(x, y, l, w, 8, m_fgColor);
}

void ClockWidget::drawAppearRL(int seg, int seq) {
    int x;
    int y;
    int l;
    int w;
    y = segLoc[seg][1];
    w = segLoc[seg][3];
    if (seg == 0 || seg == 3 || seg == 6) {
        x = segLoc[seg][0] + (20 * (4 - seq));
        l = ((seq) * 20);
    } else {
        x = segLoc[seg][0] + (25 * (4 - seq));
        l = segLoc[seg][2];
        if (seq > 0 && seq < 4) {
            y = y + 8;
            w = w - 20;
        }
    }
    m_manager.spr_fillRoundRect(x, y, l, w, 8, m_fgColor);
}

void ClockWidget::drawAppearLR(int seg, int seq) {
    int x;
    int y;
    int l;
    int w;
    y = segLoc[seg][1];
    w = segLoc[seg][3];
    if (seg == 0 || seg == 3 || seg == 6) {
        x = segLoc[seg][0];
        l = ((seq) * 20);
    } else {
        x = segLoc[seg][0] - (25 * (4 - seq));
        l = segLoc[seg][2];
        if (seq > 0 && seq < 4) {
            y = y + 8;
            w = w - 20;
        }
    }
    m_manager.spr_fillRoundRect(x, y, l, w, 8, m_fgColor);
}

void ClockWidget::drawDisappearLR(int seg, int seq) {
    int x;
    int y;
    int l;
    int w;
    y = segLoc[seg][1];
    w = segLoc[seg][3];
    if (seg == 0 || seg == 3 || seg == 6) {
        x = segLoc[seg][0] + (20 * (seq));
        l = ((4 - seq) * 20);
    } else {
        x = segLoc[seg][0] + (25 * (seq));
        l = segLoc[seg][2];
        if (seq > 0 && seq < 4) {
            y = y + 8;
            w = w - 20;
        }
    }
    m_manager.spr_fillRoundRect(x, y, l, w, 8, m_fgColor);
}

void ClockWidget::drawDisappearRL(int seg, int seq) {
    int x;
    int y;
    int l;
    int w;
    y = segLoc[seg][1];
    w = segLoc[seg][3];
    if (seg == 0 || seg == 3 || seg == 6) {
        x = segLoc[seg][0];
        l = ((4 - seq) * 20);
    } else {
        x = segLoc[seg][0] + (25 * (seq));
        l = segLoc[seg][2];
        if (seq > 0 && seq < 4) {
            y = y + 8;
            w = w - 20;
        }
    }
    m_manager.spr_fillRoundRect(x, y, l, w, 8, m_fgColor);
}

void ClockWidget::displayMorphDigit(int orb, const String m_digitlast, const String m_digit, uint32_t color) {
    int tranForSeg;
    int m_dig = 0;
    if (m_digit != " ")
        m_dig = m_digit.toInt();

    for (int lv_anim = 0; lv_anim < 5; lv_anim++) { // Loop for the animation
        m_manager.spr_fillSprite(TFT_BLACK); // Clear sprite
        for (int lv_seg = 0; lv_seg < 7; lv_seg++) { // Draw each segment in the sprite
            if (m_digit == " " && m_digitlast == " ")
                tranForSeg = 9;
            else if (m_digit == " " || m_digitlast == " ")
                tranForSeg = trans_0_B[m_dig][lv_seg];
            else if (m_digit == m_digitlast)
                tranForSeg = 0;
            else if (m_digit == "0") {
                if (m_digitlast == "2")
                    tranForSeg = trans_0_2[m_dig][lv_seg];
                if (m_digitlast == "3")
                    tranForSeg = trans_0_3[m_dig][lv_seg];
                if (m_digitlast == "5")
                    tranForSeg = trans_0_5[m_dig][lv_seg];
                if (m_digitlast == "9")
                    tranForSeg = trans_0_9[m_dig][lv_seg];
            } else if (m_digit == "1") {
                if (m_digitlast == "0")
                    tranForSeg = trans_0_9[m_dig][lv_seg];
                if (m_digitlast == "2")
                    tranForSeg = trans_0_2[m_dig][lv_seg];
            } else
                tranForSeg = trans_0_9[m_dig][lv_seg];

            switch (tranForSeg) { // Use the transition on each segment
            case 0:
                if (digits[m_dig][lv_seg] == 1)
                    drawSegment(lv_seg, m_fgColor);
                break; // Draw the segment
            case 1:
                drawDisappearBT(lv_seg, lv_anim);
                break;
            case 2:
                drawAppearTB(lv_seg, lv_anim);
                break;
            case 3:
                drawDisappearRL(lv_seg, lv_anim);
                break;
            case 4:
                drawDisappearLR(lv_seg, lv_anim);
                break;
            case 5:
                drawAppearRL(lv_seg, lv_anim);
                break;
            case 6:
                drawAppearLR(lv_seg, lv_anim);
                break;
            case 7:
                drawDisappearTB(lv_seg, lv_anim);
                break;
            case 8:
                drawAppearBT(lv_seg, lv_anim);
                break;
            case 9:
                break;
            }
        }
        delay(m_morphAnimDelay);
        m_manager.spr_pushSprite(62, 28);
        // Serial.printf("displayDigit : %i + %s + %s + %d + %d\n",orb, m_digitlast, m_digit, m_dig, tranForSeg);
    }
}