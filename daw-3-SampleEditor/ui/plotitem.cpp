#include "plotitem.h"
#include "core/measure.h"
#include "core/sampleeditor.h"
#include "core/screeninterface.h"
#include <QCursor>
#include <QtMath>
#include <core/cliparea.h>
PlotItem::PlotItem(AreaInfo* areaInfo)
    : IPainterItem()
    , _areaInfo(areaInfo)
{
}

void PlotItem::draw(IPainter* painter)
{

    double final_size = 0;
    double stepDuration = 0;
    double offsetPix = 0;
    double offsetTime = 0;

    auto _currentDuration = _areaInfo->clipDuration();
    auto _lastWidth = _areaInfo->lastWidth();
    auto _startTime = _areaInfo->startTime();
    auto _lenghtOfBeat = _areaInfo->lenghtOfBeat();
    auto _beatPerBar = _areaInfo->beatPerBar();

    auto _tempo = _areaInfo->tempo();

    auto adaptiveGrid = _areaInfo->adaptiveGrid();
    auto fixedGrid = _areaInfo->fixedGrid();

    if (adaptiveGrid == AdaptiveGrid_Off && fixedGrid == AdaptiveGrid_Off) {

        adaptiveGrid = _adaptiveGrid;
        fixedGrid = _fixedGrid;
        _offGrid = true;
    } else {
        _offGrid = false;
        adaptiveGrid = _areaInfo->adaptiveGrid();
        fixedGrid = _areaInfo->fixedGrid();
        _adaptiveGrid = adaptiveGrid;
        _fixedGrid = fixedGrid;
    }

    int Y = _rect.y();
    int B = _rect.bottom();
    int H = _rect.height();
    int T = _rect.top();

    double startTime = _startTime;
    double clipDuration = _currentDuration;

    int lob = _lenghtOfBeat;
    int bpb = _beatPerBar;
    auto m = Measure::timeToMeasure(clipDuration, lob, bpb, _tempo);

    double beatDuration = m.getBeatDuration();
    double barDuration = m.getBarDuration();
    double sixDuration = m.getSixteenthDuration();
    int min_color = ScreenInterface::scaleSize2(80, _areaInfo->zoomFactor());
    int max_color_2 = ScreenInterface::scaleSize2(600, _areaInfo->zoomFactor());
    double plotStepDuration;
    double posX = 0;
    if (_lastWidth <= 0) {
        return;
    }

    float startBraceMarker = (_areaInfo->startBraceMarker() - _areaInfo->startTime())
        / _areaInfo->clipDuration() * _areaInfo->lastWidth();

    float endBraceMarker = (_areaInfo->endBraceMarker() - _areaInfo->startTime())
        / _areaInfo->clipDuration() * _areaInfo->lastWidth();

    if (adaptiveGrid != AdaptiveGrid_Off) {

        int min = ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor());
        int max = ScreenInterface::scaleSize2(50, _areaInfo->zoomFactor());

        switch (adaptiveGrid) {
        case AdaptiveGrid_Narrow:
            min = ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor());
            max = ScreenInterface::scaleSize2(40, _areaInfo->zoomFactor());
            break;
        case AdaptiveGrid_Narrowest:
            min = ScreenInterface::scaleSize2(8, _areaInfo->zoomFactor());
            max = ScreenInterface::scaleSize2(16, _areaInfo->zoomFactor());
            break;
        case AdaptiveGrid_Noraml:
            min = ScreenInterface::scaleSize2(50, _areaInfo->zoomFactor());
            max = ScreenInterface::scaleSize2(100, _areaInfo->zoomFactor());
            break;
        case AdaptiveGrid_Wide:
            min = ScreenInterface::scaleSize2(200, _areaInfo->zoomFactor());
            max = ScreenInterface::scaleSize2(400, _areaInfo->zoomFactor());
            break;
        case AdaptiveGrid_Widest:
            min = ScreenInterface::scaleSize2(400, _areaInfo->zoomFactor());
            max = ScreenInterface::scaleSize2(800, _areaInfo->zoomFactor());
            break;
        }

        if (adaptiveGrid == AdaptiveGrid_Narrow
            || adaptiveGrid == AdaptiveGrid_Narrowest
            || adaptiveGrid == AdaptiveGrid_Noraml) {
            int coeff_six = 1;
            int coeff_beat = 1;
            int coeff_bar = 1;
            final_size = (beatDuration * 1000.0) / clipDuration * _lastWidth;
            stepDuration = beatDuration;
            if (final_size < min) {

                coeff_beat = 1;
                while (final_size < min && coeff_beat < lob) {
                    stepDuration = beatDuration * coeff_beat;
                    final_size = (stepDuration * 1000.0) / clipDuration * _lastWidth;
                    coeff_beat *= 2;
                }

            } else if (final_size > max) {
                coeff_beat = 1;
                while (final_size > max) {
                    stepDuration = beatDuration / coeff_beat;
                    final_size = (stepDuration * 1000.0) / clipDuration * _lastWidth;
                    coeff_beat *= 2;
                }
            }

            coeff_bar = 1;
            if (final_size < min) {
                while (final_size < min) {
                    stepDuration = barDuration * coeff_bar;
                    final_size = (stepDuration * 1000.0) / clipDuration * _lastWidth;
                    coeff_bar *= 2;
                }
            }

            offsetTime = (qCeil((startTime) / (stepDuration * 1000.0))) * (stepDuration * 1000.0);
            offsetPix = (offsetTime - (startTime)) / clipDuration * _lastWidth;
            posX = offsetPix;

            plotStepDuration = stepDuration * 1000.0;
        }
        if (adaptiveGrid == AdaptiveGrid_Wide
            || adaptiveGrid == AdaptiveGrid_Widest) {
            int coeff_six = 0;
            int coeff_beat = 0;
            int coeff_bar = 0;

            final_size = (sixDuration * 1000.0) / clipDuration * _lastWidth;

            stepDuration = sixDuration;

            if (final_size > max) {

                coeff_six = 1;
                while (final_size > max) {
                    stepDuration = sixDuration / coeff_six;
                    final_size = (sixDuration / coeff_six * 1000.0) / clipDuration * _lastWidth;
                    coeff_six *= 2;
                }
                if (coeff_six >= 2)
                    coeff_six /= 2;

            } else {

                coeff_beat = 1;
                while (coeff_beat <= lob / 2 && final_size < min) {
                    stepDuration = beatDuration * coeff_beat;
                    final_size = (beatDuration * coeff_beat * 1000.0) / clipDuration * _lastWidth;
                    coeff_beat *= 2;
                }
                if (final_size < min) {
                    coeff_bar = 1;
                    while (final_size < min) {
                        stepDuration = barDuration * coeff_bar;
                        final_size = (barDuration * coeff_bar * 1000.0) / clipDuration * _lastWidth;
                        coeff_bar *= 2;
                    }
                }
            }

            offsetTime = (qCeil((startTime) / (stepDuration * 1000.0))) * (stepDuration * 1000.0);
            offsetPix = (offsetTime - (startTime)) / clipDuration * _lastWidth;

            plotStepDuration = stepDuration * 1000.0;

            posX = offsetPix;
        }
    }

    if (adaptiveGrid == AdaptiveGrid_Off && fixedGrid != FixedGrid_Off) {

        int gridLevel = 1;
        int baseStep = 0;
        int barStep = 1;
        int baseSubStep = 1;
        switch (fixedGrid) {
        case FixedGrid_1_8:
            gridLevel = 8;
            break;
        case FixedGrid_1_4:
            gridLevel = 4;
            break;
        case FixedGrid_1_16:
            gridLevel = 16;
            break;
        case FixedGrid_1_32:
            gridLevel = 32;
            break;
        case FixedGrid_1_2:
            gridLevel = 2;
            break;
        case FixedGrid_1:
            barStep = 1;
            break;
        case FixedGrid_2:
            barStep = 2;
            break;
        case FixedGrid_4:
            barStep = 4;
            break;
        case FixedGrid_8:
            barStep = 8;
            break;
        }

        if (lob / gridLevel < 1) {
            baseSubStep = gridLevel / lob;
            baseStep = 1;
        } else {
            baseStep = lob / gridLevel;
            baseSubStep = 1;
        }

        if (barStep >= 1 && gridLevel == 1) {
            stepDuration = barDuration * (double)barStep * 1000.0;
            final_size = stepDuration / clipDuration * _lastWidth;
            int coef = 1;
            while (final_size < ScreenInterface::instance()->freeResolution(8)) {
                stepDuration = coef * barDuration * (double)barStep * 1000.0;
                final_size = stepDuration / clipDuration * _lastWidth;
                coef *= 2;
            }

        } else {
            if (baseStep > 1 && baseSubStep == 1) {

                stepDuration = beatDuration * (double)baseStep * 1000.0;

                final_size = stepDuration / clipDuration * _lastWidth;
                int coef = 1;
                while (final_size < ScreenInterface::instance()->freeResolution(8)) {
                    stepDuration = coef * barDuration * 1000.0;
                    final_size = stepDuration / clipDuration * _lastWidth;
                    coef *= 2;
                }
            } else {

                stepDuration = beatDuration / (double)baseSubStep * 1000.0;

                final_size = stepDuration / clipDuration * _lastWidth;

                while (final_size < ScreenInterface::instance()->freeResolution(8) && baseSubStep > 0) {
                    baseSubStep /= 2;
                    if (baseSubStep > 0) {
                        stepDuration = beatDuration / (double)baseSubStep * 1000.0;
                        final_size = stepDuration / clipDuration * _lastWidth;
                    }
                }

                if (baseSubStep == 0) {
                    int k = 1;
                    while (k * beatDuration < barDuration) {
                        if (barDuration / (double)(k * beatDuration) - qFloor(barDuration / (double)(k * beatDuration)) == 0.0) {
                            stepDuration = k * beatDuration * 1000.0;
                            final_size = stepDuration / clipDuration * _lastWidth;
                            if (final_size > ScreenInterface::instance()->freeResolution(8)) {
                                break;
                            }
                        }
                        k *= 2;
                    }
                    if (final_size < ScreenInterface::instance()->freeResolution(8)) {

                        int coef = 1;
                        while (final_size < ScreenInterface::instance()->freeResolution(8)) {
                            stepDuration = coef * barDuration * 1000.0;
                            final_size = stepDuration / clipDuration * _lastWidth;
                            coef *= 2;
                        }
                    }
                }
            }
        }

        offsetTime = (qCeil((startTime) / (stepDuration))) * (stepDuration);
        offsetPix = (offsetTime - (startTime)) / clipDuration * _lastWidth;

        posX = offsetPix;

        plotStepDuration = stepDuration;

        stepDuration /= 1000.0;
    }

    if (_offGrid) {

        _areaInfo->setStepPlotDuration(0);
    } else {

        _areaInfo->setStepPlotDuration(plotStepDuration);
    }

    _areaInfo->setStartOffsetX(offsetPix);
    _areaInfo->setStepPlotSize(final_size);
    _areaInfo->setGridOff(_offGrid);

    QNanoColor grid1c = "#454545";
    QNanoColor grid1f = "#424242";
    QNanoColor grid1i = "#3F3F3F";
    QNanoColor grid1aBG = "#494949";

    QNanoColor grid2c = "#484848";
    QNanoColor grid2f = "#464646";
    QNanoColor grid2i = "#3C3C3C";
    QNanoColor grid2aBG = "#4C4C4C";

    if (_areaInfo->themeType() == AreaInfo::ThemeType_Theme2) {
        grid1c = "#6E6E6E";
        grid1f = "#696969";
        grid1i = "#5A5A5A";
        grid1aBG = "#6F6F6F";

        grid2c = "#686868";
        grid2f = "#646464";
        grid2i = "#5F5F5F";
        grid2aBG = "#747474";
    }

    QNanoColor grid3c = "#5A5A5A";
    QNanoColor grid3f = "#565656";
    QNanoColor grid3i = "#535353";
    QNanoColor grid3aBG = "#606060";

    QNanoColor grid4c = "#5F5F5F";
    QNanoColor grid4f = "#5B5B5B";
    QNanoColor grid4i = "#4F4F4F";
    QNanoColor grid4aBG = "#656565";

    if (_areaInfo->themeType() == AreaInfo::ThemeType_Theme2) {
        grid3c = "#8C8C8C";
        grid3f = "#868686";
        grid3i = "#747474";
        grid3aBG = "#8D8D8D";

        grid4c = "#858585";
        grid4f = "#808080";
        grid4i = "#7A7A7A";
        grid4aBG = "#949494";
    }

    double lastX = 0;

    m = Measure::timeToMeasure(1000, lob, bpb, _tempo);

    double barSize = (m.barDuration * 1000.0) / clipDuration * _lastWidth;
    double beatSize = (m.beatDuration * 1000.0) / clipDuration * _lastWidth;
    double stepColorSize = barSize;
    double stepColorDuration = m.barDuration;

    if (stepColorSize < min_color) {

        int step = 1;
        stepColorSize = barSize;
        stepColorDuration = m.barDuration;
        while (stepColorSize < min_color) {
            stepColorSize = pow(2, step) * barSize;
            stepColorDuration = pow(2, step) * m.barDuration;

            step++;
        }

    } else if (stepColorSize > max_color_2) {
        stepColorSize = beatSize;
        stepColorDuration = m.beatDuration;

    } else {
    }

    _areaInfo->setStepColorDuration(stepColorDuration);
    _areaInfo->setStepColorSize(stepColorSize);

    double off = startTime / clipDuration * _lastWidth;
    double EndPicX = (1 - (off / stepColorSize - qFloor(off / stepColorSize))) * stepColorSize;
    double StartPicX = 0;
    if (startTime == 0) {
        EndPicX = stepColorSize;
    }

    double realStartX = off;
    int stepCounter = qFloor(realStartX / stepColorSize);

    double currentTime = offsetTime / 1000.0;

    double endX = (startTime + clipDuration) * (_lastWidth / clipDuration);

    painter->beginPath();

    painter->setLineWidth(1);
    painter->setLineCap(IPainter::CAP_ROUND);
    painter->setLineJoin(IPainter::JOIN_ROUND);
    QMap<int, QPair<double, double>> areaList;

    auto rectSelectedTrack = _areaInfo->selectedTrackItemRect();
    rectSelectedTrack.setTop(rectSelectedTrack.top() + 3 * ScreenInterface::scaleSize2(75, _areaInfo->zoomFactor()));
    rectSelectedTrack.setBottom(rectSelectedTrack.bottom() + 3 * ScreenInterface::scaleSize2(75, _areaInfo->zoomFactor()));

    if (rectSelectedTrack.top() < 3 * ScreenInterface::scaleSize2(75, _areaInfo->zoomFactor())) {
        rectSelectedTrack.setTop(3 * ScreenInterface::scaleSize2(75, _areaInfo->zoomFactor()));
    }

    while (realStartX < endX) {

        if (_areaInfo->areaType() == AreaInfo::AreaType_ClipArea) {

            if (stepCounter % 2 == 0) {

                painter->setFillStyle(grid1aBG);
                painter->setStrokeStyle(grid1aBG);
            } else {

                painter->setFillStyle(grid2aBG);
                painter->setStrokeStyle(grid2aBG);
            }
            painter->fillRect(StartPicX, Y, EndPicX - StartPicX, B);
            if (stepCounter % 2 == 0) {

                painter->setFillStyle(grid3aBG);
                painter->setStrokeStyle(grid3aBG);
            } else {

                painter->setFillStyle(grid4aBG);
                painter->setStrokeStyle(grid4aBG);
            }

            if (rectSelectedTrack.height() > 0) {

                painter->fillRect(StartPicX, rectSelectedTrack.top(), EndPicX - StartPicX, rectSelectedTrack.bottom() - rectSelectedTrack.top());
            }

            areaList.insert(stepCounter, QPair<double, double>(StartPicX, EndPicX));

        } else if (_areaInfo->areaType() == AreaInfo::AreaType_SampleEditor) {

            areaList.insert(stepCounter, QPair<double, double>(StartPicX, EndPicX));
            if (startBraceMarker >= EndPicX) {
                if (stepCounter % 2 == 0) {

                    painter->setFillStyle(grid1aBG);
                    painter->setStrokeStyle(grid1aBG);
                } else {

                    painter->setFillStyle(grid2aBG);
                    painter->setStrokeStyle(grid2aBG);
                }
                painter->fillRect(StartPicX, Y, EndPicX - StartPicX, B - T);

            } else if (startBraceMarker <= EndPicX && startBraceMarker >= StartPicX) {
                if (stepCounter % 2 == 0) {

                    painter->setFillStyle(grid1aBG);
                    painter->setStrokeStyle(grid1aBG);
                } else {

                    painter->setFillStyle(grid2aBG);
                    painter->setStrokeStyle(grid2aBG);
                }
                painter->fillRect(StartPicX, Y, startBraceMarker - StartPicX, B - T);

                if (stepCounter % 2 == 0) {

                    painter->setFillStyle(grid3aBG);
                    painter->setStrokeStyle(grid3aBG);
                } else {

                    painter->setFillStyle(grid4aBG);
                    painter->setStrokeStyle(grid4aBG);
                }

                painter->fillRect(startBraceMarker, T, EndPicX - startBraceMarker, B - T);

            } else if (startBraceMarker <= StartPicX && EndPicX <= endBraceMarker) {

                if (stepCounter % 2 == 0) {

                    painter->setFillStyle(grid3aBG);
                    painter->setStrokeStyle(grid3aBG);
                } else {

                    painter->setFillStyle(grid4aBG);
                    painter->setStrokeStyle(grid4aBG);
                }

                painter->fillRect(StartPicX, Y, EndPicX - StartPicX, B - T);

            } else if (endBraceMarker >= StartPicX && endBraceMarker <= EndPicX) {

                if (stepCounter % 2 == 0) {

                    painter->setFillStyle(grid3aBG);
                    painter->setStrokeStyle(grid3aBG);
                } else {

                    painter->setFillStyle(grid4aBG);
                    painter->setStrokeStyle(grid4aBG);
                }

                painter->fillRect(StartPicX, T, endBraceMarker - StartPicX, B - T);

                if (stepCounter % 2 == 0) {

                    painter->setFillStyle(grid1aBG);
                    painter->setStrokeStyle(grid1aBG);
                } else {

                    painter->setFillStyle(grid2aBG);
                    painter->setStrokeStyle(grid2aBG);
                }
                painter->fillRect(endBraceMarker, Y, EndPicX - endBraceMarker, B - T);

            } else {
                if (stepCounter % 2 == 0) {

                    painter->setFillStyle(grid1aBG);
                    painter->setStrokeStyle(grid1aBG);
                } else {

                    painter->setFillStyle(grid2aBG);
                    painter->setStrokeStyle(grid2aBG);
                }
                painter->fillRect(StartPicX, Y, EndPicX - StartPicX, B - T);
            }
        }

        realStartX += (EndPicX - StartPicX) + 0.00001;

        StartPicX = EndPicX;
        EndPicX = EndPicX + stepColorSize;

        stepCounter = qFloor(realStartX / stepColorSize);
    }

    off = startTime / clipDuration * _lastWidth;
    EndPicX = (1 - (off / stepColorSize - qFloor(off / stepColorSize))) * stepColorSize;
    StartPicX = 0;
    if (startTime == 0) {
        EndPicX = stepColorSize;
    }

    realStartX = off;
    stepCounter = qFloor(realStartX / stepColorSize);

    currentTime = offsetTime / 1000.0;

    endX = (startTime + clipDuration) * (_lastWidth / clipDuration);
    posX = offsetPix;

    stepColorDuration = (clipDuration / 1000.0) / (double)_lastWidth * final_size;

    while (posX < _lastWidth) {

        painter->beginPath();

        m = Measure::timeToMeasure(currentTime * 1000.0, lob, bpb, _tempo);

        auto pairs = areaList.values();
        int index = 0;

        for (int i = 0; i < areaList.size(); i++) {

            if (pairs[i].first <= posX && posX < pairs[i].second) {
                index = i;
                break;
            }
        }

        auto stepCounter = areaList.keys()[index];

        if (stepCounter % 2 == 0) {
            if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                painter->setStrokeStyle(grid1i);
            } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                painter->setStrokeStyle(grid1f);
            } else {
                painter->setStrokeStyle(grid1c);
            }

        } else {
            if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                painter->setStrokeStyle(grid2i);
            } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                painter->setStrokeStyle(grid2f);
            } else {
                painter->setStrokeStyle(grid2c);
            }
        }

        painter->setLineWidth(1);
        if (_offGrid) {
            painter->drawDashedLine(posX, Y, posX, B, 5);
        } else {
            painter->drawLine(posX, Y, posX, B);
        }
        painter->stroke();
        painter->beginPath();

        if (_areaInfo->areaType() == AreaInfo::AreaType_ClipArea) {

            if (stepCounter % 2 == 0) {
                if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                    painter->setStrokeStyle(grid3i);
                } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                    painter->setStrokeStyle(grid3f);
                } else {
                    painter->setStrokeStyle(grid3c);
                }
            } else {
                if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                    painter->setStrokeStyle(grid4i);
                } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                    painter->setStrokeStyle(grid4f);
                } else {
                    painter->setStrokeStyle(grid4c);
                }
            }

            if (rectSelectedTrack.height() > 0) {
                if (_offGrid) {
                    painter->drawDashedLine(posX, rectSelectedTrack.top(), posX, rectSelectedTrack.bottom(), 5);
                } else {
                    painter->drawLine(posX, rectSelectedTrack.top(), posX, rectSelectedTrack.bottom());
                }
            }
            painter->stroke();

        } else if (_areaInfo->areaType() == AreaInfo::AreaType_SampleEditor) {
            painter->beginPath();

            if (posX >= startBraceMarker && posX <= endBraceMarker) {

                if (stepCounter % 2 == 0) {
                    if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                        painter->setStrokeStyle(grid3i);
                    } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                        painter->setStrokeStyle(grid3f);
                    } else {
                        painter->setStrokeStyle(grid3c);
                    }
                } else {
                    if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                        painter->setStrokeStyle(grid4i);
                    } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                        painter->setStrokeStyle(grid4f);
                    } else {
                        painter->setStrokeStyle(grid4c);
                    }
                }
            } else {

                if (stepCounter % 2 == 0) {
                    if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                        painter->setStrokeStyle(grid1i);
                    } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                        painter->setStrokeStyle(grid1f);
                    } else {
                        painter->setStrokeStyle(grid1c);
                    }

                } else {
                    if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                        painter->setStrokeStyle(grid2i);
                    } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                        painter->setStrokeStyle(grid2f);
                    } else {
                        painter->setStrokeStyle(grid2c);
                    }
                }
            }

            if (_offGrid) {
                painter->drawDashedLine(posX, T, posX, B, 5);
            } else {
                painter->drawLine(posX, T, posX, B);
            }
            painter->stroke();
        }

        lastX = posX;
        posX += final_size;

        currentTime += stepColorDuration;
    }

    painter->stroke();

    if (_areaInfo->loopFocusActive()) {
        painter->beginPath();
        painter->setLineWidth(1);
        painter->setStrokeStyle("#EAEAEA");
        painter->drawLine(startBraceMarker, _areaInfo->scrubAreaTop(), startBraceMarker, B);
        painter->drawLine(endBraceMarker, _areaInfo->scrubAreaTop(), endBraceMarker, B);
        painter->stroke();
    }
}

int PlotItem::trackIndex() const
{
    return _trackIndex;
}

void PlotItem::setTrackIndex(int newTrackIndex)
{
    _trackIndex = newTrackIndex;
}

int PlotItem::mouseReleaseEvent(QMouseEvent* event)
{
    Q_EMIT sigSetCursor(QCursor(Qt::ArrowCursor));
    return 1;
}
