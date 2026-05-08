#include "automationitem.h"
#include "core/sampleeditor.h"
#include <core/cliparea.h>
#include <ui/trackitem.h>
AutomationItem::AutomationItem(int trackIndex,
    QSharedPointer<Automation> automation, AreaInfo* areaInfo)
    : IPainterItem()
    , _areaInfo(areaInfo)
    , _trackIndex(trackIndex)
    , _automation(automation)

{
    _currentValue = "";

    _timerCheckPressAndHold.setInterval(100);
    connect(&_timerCheckPressAndHold, &QTimer::timeout, this, &AutomationItem::sltTimerMouse);

    _timerCheckDoubleClick.setInterval(300);
    connect(&_timerCheckDoubleClick, &QTimer::timeout, this, &AutomationItem::sltTimerDoubleClicked);

    _timeMoving.setInterval(30);
    connect(&_timeMoving, &QTimer::timeout, this, &AutomationItem::sltTimerMoving);
    _timeMoving.start();

    _timerCursorPosition.setInterval(30);
    connect(&_timerCursorPosition, &QTimer::timeout, this, &AutomationItem::sltTimerCheckPosition);

    connect(_automation.get(), &Automation::sigLinesChanged, this, &AutomationItem::sltLinesChanged);

    connect(_areaInfo, &AreaInfo::sigLastWidthChanged, this, &AutomationItem::sigUpdate);
    connect(_areaInfo, &AreaInfo::sigZoomLevelChanged, this, &AutomationItem::sigUpdate);
    connect(_areaInfo, &AreaInfo::sigClipPropertiesChanged1, this, &AutomationItem::sigUpdate);
    connect(_areaInfo, &AreaInfo::loopFocusActiveChanged, this, &AutomationItem::sigUpdate);
    connect(_areaInfo, &AreaInfo::playheadMarkerChanged, this, &AutomationItem::sigUpdate);
}

AutomationItem::~AutomationItem()

{

    qDebug() << "~AutomationItem()";
}

void AutomationItem::checkEvents()
{

    //    return;
    //    QVector<QLineF>
    //        ranges = _automation->getRanges(_liveMovingEnabled);

    //    if (qIsInf(-ranges[0].p1().x())) {

    //        ranges[0].setP1(QPointF(0, ranges[0].p1().y()));
    //    }
    //    if (qIsInf(ranges[ranges.size() - 1].p2().x())) {

    //        ranges[ranges.size() - 1].setP2(QPointF(_areaInfo->lastWidth(), ranges[ranges.size() - 1].p2().y()));
    //    }

    //    if (_mouseClick) {
    //        _mouseClick = false;

    //        _automation->findCurrentActives(_mousePressX, _mousePressY, _currentRangeIndex, _pointActive, _innerActive, _outerActive);

    //        if (_currentRangeIndex >= 0 && (_innerActive == true || _outerActive == true || _pointActive == true)) {

    //            if (_pointActive == true) {

    //                int leftIndex, rightIndex;
    //                _automation->find_lines_by_point(QPointF(_mousePressX, _mousePressY), leftIndex, rightIndex);
    //                _currentIndex = rightIndex;
    //                bool selected_point = _automation->isSelectedPoint(_currentIndex);

    //                if (selected_point) {

    //                } else {
    //                    _automation->clearSelectedLines();
    //                }

    //            } else {
    //                _automation->clearSelectedLines();
    //            }

    //        } else {
    //        }

    //        if (_currentRangeIndex >= 0) {
    //            if (_innerActive) {

    //                if (_areaInfo->altKeyPressed() == false) {
    //                    float curve_n = 1;
    //                    float curve_m = 1;

    //                    _automation->getCurve(_currentRangeIndex, curve_n, curve_m, _liveMovingEnabled);

    //                    if (curve_n == 1.0 && curve_m == 1.0) {
    //                        QPointF mapped = MathHelper1::mapPointToLine(ranges[_currentRangeIndex].p1(), ranges[_currentRangeIndex].p2(), QPointF(_currentMouseHoverX, _currentMouseHoverY));

    //                        saveInUndoItems();
    //                        _automation->addLine(mapped);

    //                        _currentRangeIndex = _automation->findRangeIndex(_mousePressX, _mousePressY); // findCurrentLineIndex(_currentMousePressX, _currentMousePressY);
    //                        _automation->createSnapLine(true);

    //                    } else {

    //                        QPointF a = ranges[_currentRangeIndex].p2();
    //                        QPointF b = ranges[_currentRangeIndex].p1();

    //                        float A, B, centerX, centerY;
    //                        if (curve_n < 0) {
    //                            centerX = a.x();
    //                            centerY = b.y();

    //                            QPointF bb = QPointF(b.x(), b.y());
    //                            QPointF aa = QPointF(a.x(), a.y());

    //                            B = -(aa.y() - centerY);
    //                            A = -(centerX - bb.x());

    //                        } else {

    //                            centerX = b.x();
    //                            centerY = a.y();

    //                            QPointF bb = QPointF(b.x(), b.y());
    //                            QPointF aa = QPointF(a.x(), a.y());

    //                            A = (aa.x() - centerX);
    //                            B = (centerY - bb.y());
    //                        }

    //                        float p, q, r, s;

    //                        auto result = MathHelper1::superEllipseXtoY(ranges[_currentRangeIndex].p2(), ranges[_currentRangeIndex].p1(), curve_n, curve_m,
    //                            _mousePressX, _mousePressY);

    //                        double x = result.x();
    //                        double y = result.y();

    //                        MathHelper1::findRoot(qAbs(curve_n), qAbs(curve_m), 0, 0, qAbs(A), qAbs(B), 1000, x - centerX, centerY - y, p, q, r, s);

    //                        qDebug() << qSetRealNumberPrecision(10) << "eclips_order: " << curve_n << "x_center:" << 0 << "y_center:" << 0
    //                                 << "x_radius:" << qAbs(A) << "y_radius:" << qAbs(B) << "sample_number:" << 1000 << "x_point:"
    //                                 << x - centerX << "y_point:" << centerY - y << "p:" << p << "q:" << q << "r:" << r << "s:" << s;

    //                        if (curve_n < 0) {
    //                            saveInUndoItems();
    //                            _automation->addLine(QPointF(x, y), -p, -q, -r, -s);
    //                        } else {
    //                            saveInUndoItems();
    //                            _automation->addLine(QPointF(x, y), p, q, r, s);
    //                        }
    //                        _automation->createSnapLine(true);
    //                        // Not Impelemeted
    //                    }
    //                }
    //            } else if (_pointActive) {

    //                int line_index = _automation->find_line(
    //                    ranges[_currentRangeIndex].p1().x(),
    //                    ranges[_currentRangeIndex].p1().y(),
    //                    ranges[_currentRangeIndex].p2().x(),
    //                    ranges[_currentRangeIndex].p2().y());

    //                auto state = _automation->getLinesState();

    //                if (_automation->removeLine(line_index)) {
    //                    saveInUndoItems(state);
    //                    _automation->createSelectedPoints(_areaInfo->selectionArea()->area().left(), _areaInfo->selectionArea()->area().right());
    //                    _currentRangeIndex = -1;
    //                    if (_automation->size() == 0) {
    //                        setNormalized(_automation->defaultNormalized());
    //                    }
    //                }
    //            }
    //        }
    //    } else if (_mousePressAndHold) {

    //        if (_mousePressAndHold2) {
    //            _mousePressAndHold2 = false;
    //            _automation->createSnapLine(true);

    //            _curveChangingEnabled = false;
    //            if (_areaInfo->altKeyPressed()) {
    //                _curveChangingEnabled = true;
    //            }

    //            _automation->findCurrentActives(_mousePressX, _mousePressY, _currentRangeIndex, _pointActive, _innerActive, _outerActive);

    //            if (_innerActive) {
    //                _currentIndex = -1;
    //            }

    //            if (_pointActive) {
    //                int leftIndex, rightIndex;
    //                _automation->find_lines_by_point(QPointF(_mousePressX, _mousePressY), leftIndex, rightIndex);
    //                _currentIndex = rightIndex;

    //                _excludePoint1 = leftIndex;
    //                _excludePoint2 = rightIndex;

    //                _currentSelectedPointIndex = _automation->find_selected_line_start_point(_mousePressX, _mousePressY);
    //                _currentSelectedPointIndex--;
    //            }

    //            if (_outerActive) {

    //                if (_selectedLinesMoving == true || (_automation->inSelectedLine(QPointF(_currentMouseHoverX, _currentMouseHoverY)) && _currentRangeIndex >= 0)) {
    //                    // Moving selected line
    //                } else {

    //                    _currentMovingIndex = _automation->find_line_start_point(ranges[_currentRangeIndex].p1().x(), ranges[_currentRangeIndex].p1().y());

    //                    if (_currentMovingIndex < 0 || _currentMovingIndex >= _automation->getLines().size()) {

    //                        _currentLinePoint.setX(_mousePressX);
    //                        _currentLinePoint.setY(_mousePressY);

    //                        //"A is point on the line"
    //                        QPointF correctedPoint = MathHelper1::mapPointToLine(ranges[_currentRangeIndex].p1(), ranges[_currentRangeIndex].p2(),
    //                            QPointF(_currentLinePoint.x(), _currentLinePoint.y()));

    //                        // According to A point we have two lines, on the current line
    //                        auto lines = MathHelper1::pointToD(ranges[_currentRangeIndex].p1(), ranges[_currentRangeIndex].p2(), correctedPoint);
    //                        QPointF p1 = ranges[_currentRangeIndex].p1();
    //                        QPointF p2 = ranges[_currentRangeIndex].p2();
    //                        _currentLineSlope = MathHelper1::slope(p1, p2);

    //                        _currentLinePointD1 = lines.first;
    //                        _currentLinePointD2 = lines.second;

    //                        _currentMovingIndex1 = -1;
    //                        _currentMovingIndex2 = -1;

    //                        _currentMovingIndex1 = _automation->find_line_start_point(ranges[_currentRangeIndex].p1().x(), ranges[_currentRangeIndex].p1().y());
    //                        _currentMovingIndex2 = _automation->find_line_start_point(ranges[_currentRangeIndex].p2().x(), ranges[_currentRangeIndex].p2().y());

    //                    } else {
    //                        _automation->initMoveLine(_currentMovingIndex);
    //                    }
    //                }
    //            }

    //            bool selected_point = _automation->isSelectedPoint(_currentIndex);
    //            bool selected_line = _automation->inSelectedLine(QPointF(_currentMouseHoverX, _currentMouseHoverY));
    //            if (_currentRangeIndex >= 0 && (_pointActive == true || _innerActive == true || _outerActive == true)) {

    //                if (_innerActive && _areaInfo->altKeyPressed() == false) {
    //                    _automation->clearSelectedLines();

    //                } else {
    //                    if (selected_point || selected_line) {

    //                    } else {

    //                        if (_areaInfo->altKeyPressed() == false) {
    //                            _automation->clearSelectedLines();
    //                        }
    //                    }
    //                }

    //            } else {
    //            }
    //        }

    //        if (_curveChangingEnabled) {
    //            if (_currentRangeIndex >= 0) {

    //                if (ranges[_currentRangeIndex].p1().y() < ranges[_currentRangeIndex].p2().y()) {
    //                    float Y1 = ranges[_currentRangeIndex].p1().y();
    //                    float Y2 = ranges[_currentRangeIndex].p2().y();

    //                    float mid = qAbs(Y2 - Y1) / 2.0 + Y1;

    //                    int index = _automation->find_line(ranges[_currentRangeIndex].p1().x(), ranges[_currentRangeIndex].p1().y(), ranges[_currentRangeIndex].p2().x(), ranges[_currentRangeIndex].p2().y());

    //                    float curve_n = 1;
    //                    float curve_m = 1;
    //                    bool valid = _automation->getCurve(_currentRangeIndex, curve_n, curve_m, _liveMovingEnabled);

    //                    if (index >= 0) {

    //                        if (_currentMouseY < mid) {
    //                            curve_n = 9.0 / (Y1 - mid) * (_currentMouseY - mid) + 1;
    //                            curve_m = 9.0 / (Y1 - mid) * (_currentMouseY - mid) + 1;

    //                        } else if (_currentMouseY > mid) {
    //                            curve_n = -9.0 / (Y2 - mid) * (_currentMouseY - mid) - 1;
    //                            curve_m = -9.0 / (Y2 - mid) * (_currentMouseY - mid) - 1;

    //                        } else {
    //                            curve_n = 1;
    //                            curve_m = 1;
    //                        }

    //                        if (curve_n <= -4.5) {
    //                            curve_n = -4.5;
    //                        }
    //                        if (curve_m <= -4.5) {
    //                            curve_m = -4.5;
    //                        }
    //                        if (curve_n > 4.5)
    //                            curve_n = 4.5;

    //                        if (curve_m > 4.5)
    //                            curve_m = 4.5;

    //                        _automation->setCurve(index, curve_n, curve_m);
    //                    }

    //                } else {

    //                    float Y1 = ranges[_currentRangeIndex].p1().y();
    //                    float Y2 = ranges[_currentRangeIndex].p2().y();

    //                    float mid = qAbs(Y2 - Y1) / 2.0 + Y2;

    //                    int index = _automation->find_line(ranges[_currentRangeIndex].p1().x(), ranges[_currentRangeIndex].p1().y(), ranges[_currentRangeIndex].p2().x(), ranges[_currentRangeIndex].p2().y());
    //                    float curve_n = 1;
    //                    float curve_m = 1;
    //                    bool valid = _automation->getCurve(_currentRangeIndex, curve_n, curve_m, _liveMovingEnabled);

    //                    if (index >= 0) {
    //                        if (_currentMouseY < mid) {
    //                            curve_n = -9.0 / (Y2 - mid) * (_currentMouseY - mid) - 1;
    //                            curve_m = -9.0 / (Y2 - mid) * (_currentMouseY - mid) - 1;
    //                        } else if (_currentMouseY > mid) {
    //                            curve_n = 9.0 / (Y1 - mid) * (_currentMouseY - mid) + 1;
    //                            curve_m = 9.0 / (Y1 - mid) * (_currentMouseY - mid) + 1;
    //                        } else {
    //                            curve_n = 1;
    //                            curve_m = 1;
    //                        }

    //                        if (curve_n <= -4.5) {
    //                            curve_n = -4.5;
    //                        }
    //                        if (curve_m <= -4.5) {
    //                            curve_m = -4.5;
    //                        }
    //                        if (curve_n > 4.5)
    //                            curve_n = 4.5;

    //                        if (curve_m > 4.5)
    //                            curve_m = 4.5;

    //                        _automation->setCurve(index, curve_n, curve_m);
    //                    }
    //                }
    //            }
    //        } else {

    //            if (_pointActive) {

    //                // qDebug() << "_pointActive2" << _automation->isSelectedPoint(_currentIndex) << _trackIndex;
    //                if ((_selectedPointsMoving == true || _automation->isSelectedPoint(_currentIndex))) {

    //                    _selectedPointsMoving = true;
    //                    _liveMovingEnabled = true;

    //                    double final_xpos = _currentMouseX;
    //                    double final_ypos = _currentMouseY;
    //                    _automation->createSnapLine(false);

    //                    _automation->findSnap(_currentMouseX, _currentMouseY, final_xpos, final_ypos);

    //                    if (_saveEditState) {
    //                        _saveEditState = false;
    //                        saveInUndoItems();
    //                    }

    //                    _automation->moveSelectedPoints(QPointF(final_xpos, _currentMouseY));

    //                } else {

    //                    _automation->createSnapLine(false, _excludePoint1, _excludePoint2);

    //                    _automation->createSelectedPoints(_areaInfo->selectionArea()->area().left(), _areaInfo->selectionArea()->area().right());
    //                    _liveMovingEnabled = true;

    //                    double final_xpos = _currentMouseX;
    //                    double final_ypos = _currentMouseY;

    //                    if (_areaInfo->altKeyPressed()) {

    //                    } else {
    //                        _automation->findSnap(_currentMouseX, _currentMouseY, final_xpos, final_ypos);
    //                    }

    //                    if (final_ypos < _automation->getWindowRect().top()) {
    //                        final_ypos = _automation->getWindowRect().top();
    //                    } else if (final_ypos > _automation->getWindowRect().bottom()) {
    //                        final_ypos = _automation->getWindowRect().bottom();
    //                    } else {
    //                        final_ypos = _currentMouseY;
    //                    }

    //                    if (_saveEditState) {
    //                        _saveEditState = false;
    //                        saveInUndoItems();
    //                    }

    //                    _automation->editLine(_currentIndex, final_xpos, final_ypos);

    //                    bool result = _automation->correctTemplateLines(_currentIndex);

    //                    int index = _automation->find_line_start_point(final_xpos, final_ypos);
    //                    if (index >= 0) {

    //                        float pixelX;
    //                        float pixelY;
    //                        _automation->line2Pixel(index, pixelX, pixelY);
    //                        if (result)
    //                            _currentRangeIndex = _automation->findRangeIndex(pixelX, pixelY);
    //                    }
    //                }
    //            }

    //            if (_innerActive) {

    //                _liveMovingEnabled = true;

    //                if (_saveEditState) {
    //                    _saveEditState = false;
    //                    saveInUndoItems();
    //                }

    //                _automation->createSnapLine(false);
    //                if (_currentRangeIndex >= 0 && _currentIndex < 0) {

    //                    float curve_n = 1, curve_m = 1;
    //                    bool valid = _automation->getCurve(_currentRangeIndex, curve_n, curve_m, _liveMovingEnabled);

    //                    if (curve_n == 1.0 && curve_m == 1.0) {

    //                        QPointF correctedPoint = MathHelper1::mapPointToLine(ranges[_currentRangeIndex].p1(), ranges[_currentRangeIndex].p2(),
    //                            QPointF(_currentMouseHoverX, _currentMouseHoverY));

    //                        double time = _automation->pixel2Time(correctedPoint.x());

    //                        _automation->addLine(time, correctedPoint.y());

    //                        _automation->createSnapLine(true);
    //                        _currentIndex = _automation->find_line_start_point(correctedPoint.x(), correctedPoint.y());

    //                        _pointActive = true;
    //                        _innerActive = false;
    //                        _outerActive = false;

    //                    } else {
    //                        QPointF a = ranges[_currentRangeIndex].p2();
    //                        QPointF b = ranges[_currentRangeIndex].p1();

    //                        float A, B, centerX, centerY;
    //                        if (curve_n < 0) {
    //                            centerX = a.x();
    //                            centerY = b.y();

    //                            QPointF bb = QPointF(b.x(), b.y());
    //                            QPointF aa = QPointF(a.x(), a.y());

    //                            B = -(aa.y() - centerY);
    //                            A = -(centerX - bb.x());

    //                        } else {

    //                            centerX = b.x();
    //                            centerY = a.y();

    //                            QPointF bb = QPointF(b.x(), b.y());
    //                            QPointF aa = QPointF(a.x(), a.y());

    //                            A = (aa.x() - centerX);
    //                            B = (centerY - bb.y());
    //                        }

    //                        float p, q, r, s;

    //                        auto result = MathHelper1::superEllipseXtoY(ranges[_currentRangeIndex].p2(), ranges[_currentRangeIndex].p1(), curve_n, curve_m,
    //                            _mousePressX, _mousePressY);

    //                        double x = result.x();
    //                        double y = result.y();

    //                        MathHelper1::findRoot(qAbs(curve_n), qAbs(curve_m), 0, 0, qAbs(A), qAbs(B), 1000, x - centerX, centerY - y, p, q, r, s);

    //                        qDebug() << qSetRealNumberPrecision(10) << "eclips_order: " << curve_n << "x_center:" << 0 << "y_center:" << 0
    //                                 << "x_radius:" << qAbs(A) << "y_radius:" << qAbs(B) << "sample_number:" << 1000 << "x_point:"
    //                                 << x - centerX << "y_point:" << centerY - y << "p:" << p << "q:" << q << "r:" << r << "s:" << s;

    //                        if (curve_n < 0) {
    //                            saveInUndoItems();
    //                            _automation->addLine(QPointF(x, y), -p, -q, -r, -s);

    //                        } else {
    //                            saveInUndoItems();
    //                            _automation->addLine(QPointF(x, y), p, q, r, s);
    //                        }
    //                        _automation->createSnapLine(true);

    //                        _currentIndex = _automation->find_line_start_point(x, y);
    //                        _pointActive = true;
    //                        _innerActive = false;
    //                        _outerActive = false;
    //                    }
    //                }

    //                QPointF correctedPoint = MathHelper1::mapPointToLine(ranges[_currentRangeIndex].p1(), ranges[_currentRangeIndex].p2(),
    //                    QPointF(_currentMouseHoverX, _currentMouseHoverY));

    //                double final_xpos = correctedPoint.x();
    //                double final_ypos = correctedPoint.y();

    //                if (_areaInfo->altKeyPressed()) {

    //                } else {
    //                    _automation->findSnap(_currentMouseX, _currentMouseY, final_xpos, final_ypos);
    //                }

    //                if (final_ypos < _automation->getWindowRect().top()) {
    //                    final_ypos = _automation->getWindowRect().top();
    //                } else if (final_ypos > _automation->getWindowRect().bottom()) {
    //                    final_ypos = _automation->getWindowRect().bottom();
    //                } else {
    //                }

    //                _automation->editLine(_currentIndex, final_xpos, final_ypos);

    //                bool result = _automation->correctTemplateLines(_currentIndex);

    //                int rightIndex;
    //                int leftIndex;
    //                _automation->find_lines_by_point(QPointF(final_xpos, final_ypos), leftIndex, rightIndex);
    //                _currentIndex = rightIndex;

    //                if (rightIndex >= 0) {
    //                    float pixelX;
    //                    float pixelY;
    //                    _automation->line2Pixel(rightIndex, pixelX, pixelY);
    //                    if (result) {
    //                        _currentRangeIndex = _automation->findRangeIndex(pixelX, pixelY);
    //                    }
    //                }
    //            }

    //            if (_outerActive) {

    //                if (_selectedLinesMoving == true || (_automation->inSelectedLine(QPointF(_currentMouseHoverX, _currentMouseHoverY)) && _currentRangeIndex >= 0)) {
    //                    _selectedLinesMoving = true;
    //                    if (_areaInfo->altKeyPressed()) {

    //                        if (_saveEditState) {
    //                            _saveEditState = false;
    //                            saveInUndoItems();
    //                        }

    //                        _automation->moveSelectedLines(QPointF(_currentMouseX, _currentMouseY));
    //                    } else {
    //                        _automation->createSnapLine(false);
    //                        float final_xpos = _currentMouseX;
    //                        float final_ypos = _currentMouseY;
    //                        float firstPointX = _automation->findFirstPointInSelection(QPointF(_currentMouseX, _currentMouseY));
    //                        _automation->findSnap(firstPointX, final_xpos);
    //                        float curPoint = _automation->convertFirstPointToCurrentPoint(QPointF(final_xpos, _currentMouseY));

    //                        if (_saveEditState) {
    //                            _saveEditState = false;
    //                            saveInUndoItems();
    //                        }

    //                        _automation->moveSelectedLines(QPointF(curPoint, final_ypos));
    //                    }

    //                } else if (_liveMovingEnabled || _currentRangeIndex >= 0) {

    //                    _liveMovingEnabled = true;
    //                    _automation->createSnapLine(false);

    //                    if (_currentMovingIndex < 0) {

    //                        auto point2 = MathHelper1::DtoPoint2(_currentLineSlope,
    //                            QPoint(_currentMouseX, _currentMouseY), _currentLinePointD1, _currentLinePointD2);

    //                        double final_xpos_2 = _currentMouseX;
    //                        double final_yPos_2 = point2.y();
    //                        _automation->createSnapLine(false);
    //                        _automation->findSnap(point2.x(), point2.y(), final_xpos_2, final_yPos_2);

    //                        if (final_yPos_2 < _automation->getWindowRect().top()) {
    //                            final_yPos_2 = _automation->getWindowRect().top();
    //                        } else if (final_yPos_2 > _automation->getWindowRect().bottom()) {
    //                            final_yPos_2 = _automation->getWindowRect().bottom();
    //                        } else {
    //                            final_yPos_2 = point2.y();
    //                        }

    //                        if (_saveEditState) {
    //                            _saveEditState = false;
    //                            saveInUndoItems();
    //                        }

    //                        _automation->editLine(_currentMovingIndex2, final_xpos_2, final_yPos_2);

    //                        bool result = _automation->correctTemplateLines(_currentMovingIndex2);
    //                        _currentRangeIndex = 0;
    //                    } else if (_currentMovingIndex >= _automation->size()) {
    //                        auto point2 = MathHelper1::DtoPoint1(_currentLineSlope,
    //                            QPoint(_currentMouseX, _currentMouseY), 0, _currentLinePointD2);

    //                        double final_xpos_2 = 0;
    //                        double final_yPos_2 = point2.y();
    //                        _automation->createSnapLine(false);
    //                        _automation->findSnap(point2.x(), point2.y(), final_xpos_2, final_yPos_2);

    //                        if (final_yPos_2 < _automation->getWindowRect().top()) {
    //                            final_yPos_2 = _automation->getWindowRect().top();
    //                        } else if (final_yPos_2 > _automation->getWindowRect().bottom()) {
    //                            final_yPos_2 = _automation->getWindowRect().bottom();
    //                        } else {
    //                            final_yPos_2 = point2.y();
    //                        }

    //                        if (_saveEditState) {
    //                            _saveEditState = false;
    //                            saveInUndoItems();
    //                        }

    //                        _automation->editLine(_currentMovingIndex1, final_xpos_2, final_yPos_2);

    //                        bool result = _automation->correctTemplateLines(_currentMovingIndex1);
    //                        _currentRangeIndex = ranges.size() - 1;

    //                    } else {

    //                        double final_xpos = _currentMouseX;
    //                        double final_ypos = _currentMouseY;
    //                        _automation->createSnapLine(false);
    //                        _automation->findSnap(_currentMouseX, _currentMouseY, final_xpos, final_ypos);

    //                        if (_saveEditState) {
    //                            _saveEditState = false;
    //                            saveInUndoItems();
    //                        }
    //                        auto line = _automation->moveLine(_currentMovingIndex, QPointF(final_xpos, final_ypos));
    //                        _currentRangeIndex = _automation->findRangeIndex(line.p1().x(), line.p1().y(), line.p2().x(), line.p2().y(), true);
    //                    }
    //                }
    //            }
    //        }
    //    } else {
    //        // Hover

    //        if (_rect.contains(QPoint(_currentMouseHoverX, _currentMouseHoverY))) {
    //            _automation->findCurrentActives(_currentMouseHoverX, _currentMouseHoverY, _currentRangeIndex, _pointActive, _innerActive, _outerActive);

    //        } else {
    //            _currentRangeIndex = -1;
    //            _pointActive = false;
    //            _innerActive = false;
    //            _outerActive = false;
    //        }

    //        if (_innerActive && _areaInfo->altKeyPressed()) {
    //            _outerActive = true;
    //            _innerActive = false;
    //            _pointActive = false;
    //        }
    //    }

    //    if (_mouseClick == false && _mouseDoubleClick == true) {
    //        _mouseDoubleClick = false;

    //        double time = _automation->pixel2Time(_mousePressX);

    //        saveInUndoItems();
    //        _automation->addLine(time, _currentMouseY);

    //        _automation->createSnapLine(true);

    //        int rightIndex;
    //        int leftIndex;
    //        _automation->find_lines_by_point(QPointF(_mousePressX, _mousePressY), leftIndex, rightIndex);

    //        _currentIndex = rightIndex;

    //        _automation->correctTemplateLines(_currentIndex);

    //        _pointActive = true;
    //        _innerActive = false;
    //        _outerActive = false;
    //    }
}

bool AutomationItem::showEnvelopes() const
{
    return _showEnvelopes;
}

void AutomationItem::setShowEnvelopes(bool showEnvelopes)
{
    _showEnvelopes = showEnvelopes;
}

void AutomationItem::saveInUndoItems()
{

    if (automation()->type() == "AudioObject") {
        return;
    }
    QJsonArray array;
    QJsonObject jObject2;
    jObject2["track_index"] = _trackIndex;
    jObject2["sub_track_index"] = automation()->getSubTrackIndex();
    jObject2["menu"] = automation()->type();
    jObject2["sub_menu"] = automation()->subType();
    jObject2["state"] = automation()->getLinesState();
    jObject2["automation"] = automation()->enabled();
    jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
    array.append(jObject2);

    ActionStack::instance()->pushInUndo(array);
}

void AutomationItem::saveInUndoItems(QJsonArray state)
{
    if (automation()->type() == "AudioObject") {
        return;
    }

    QJsonArray array;
    QJsonObject jObject2;
    jObject2["track_index"] = _trackIndex;
    jObject2["sub_track_index"] = automation()->getSubTrackIndex();
    jObject2["state"] = state;
    jObject2["menu"] = automation()->type();
    jObject2["sub_menu"] = automation()->subType();
    jObject2["automation"] = automation()->enabled();
    jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
    array.append(jObject2);
    ActionStack::instance()->pushInUndo(array);
}

void AutomationItem::setAutomatedNormalized(double y)
{

    _automation->setAutomatedNormalized(y);
    Q_EMIT sigAutomatedNormalizedChanged();
}

void AutomationItem::setNormalized(double y)
{

    _automation->setNormalized(y);
    Q_EMIT sigNormalizedChanged();
    Q_EMIT sigUpdate();
}

bool AutomationItem::showEnvelopes()
{
    return _showEnvelopes;
}
void AutomationItem::draw(IPainter* painter)
{

    //    if (_showEnvelopes == false) {
    //        return;
    //    }

    //    if (_automation->getAsyncClear()) {
    //        _automation->clear();
    //    }

    //    painter->setStrokeStyle("#eeeeee");

    //    if (_saveingLastTemplatesLines) {
    //        _saveingLastTemplatesLines = false;
    //        _selectedLinesMoving = false;
    //        _selectedPointsMoving = false;
    //        _liveMovingEnabled = false;

    //        _automation->saveTemplateLine();
    //        _automation->createSelectedPoints(_areaInfo->selectionArea()->area().left(), _areaInfo->selectionArea()->area().right());
    //    }

    //    if (qAbs(_lastStartMarker - _areaInfo->selectionArea()->area().left()) > 0.0001
    //        || qAbs(_lastEndMarker - _areaInfo->selectionArea()->area().right()) > 0.001) {
    //        _lastEndMarker = _areaInfo->selectionArea()->area().right();
    //        _lastStartMarker = _areaInfo->selectionArea()->area().left();
    //        _automation->saveTemplateLine();
    //        _automation->createSelectedPoints(_areaInfo->selectionArea()->area().left(), _areaInfo->selectionArea()->area().right());
    //    }

    //    if (_automation->getSelectedRangeMoveEnabled() == false) {
    //        _checkStrachingActive = true;

    //        checkEvents();

    //    } else {

    //        if (_checkStrachingActive == true) {
    //            _checkStrachingActive = false;
    //        }

    //        _pointActive = false;
    //        _innerActive = false;
    //        _outerActive = false;
    //        _currentRangeIndex = -1;
    //    }

    //    if (_selectAllRangeEvent) {
    //        _selectAllRangeEvent = false;
    //        _automation->selectAll();
    //    }

    //    if (_automation->size() > 0) {

    //        drawLinesAndCircles(painter, _rect.bottom());

    //        if (_automation->enabled() == false) {

    //            int T = _automation->getWindowRect().top();
    //            int B = _automation->getWindowRect().bottom();
    //            int R = _automation->getWindowRect().right();
    //            int L = _automation->getWindowRect().left();

    //            painter->beginPath();
    //            painter->setFillStyle("#ff0000");
    //            painter->setStrokeStyle("#ff0000");
    //            painter->setLineWidth(1);

    //            double y = qAbs(_automation->normalized()) * (B - T) + T;
    //            auto line = QLineF(QPointF(L, y), QPointF(R, y));
    //            painter->drawDashedLine(line.x1(), line.y1(), line.x2(), line.y1(), 5);
    //            painter->stroke();
    //        }
    //    } else {

    //        drawDashedLine(painter);
    //    }

    //    if (_automation->inSelectedLine(QPointF(_currentMouseHoverX, _currentMouseHoverY)) && _areaInfo->altKeyPressed() == false
    //        && _automation->getSelectedRangeMoveEnabled() == false /* check for streachmoving/*/) {
    //        drawSelectedLines(painter);
    //    }

    //    if (_areaInfo->altKeyPressed() == false && _automation->getSelectedRangeMoveEnabled() == false) {
    //        drawHoverCircle(painter);
    //    }

    //    if (_mouseShapeChangingEvent) {
    //        _currentCursorShape = Qt::OpenHandCursor;
    //        Q_EMIT sigSetCursor(QCursor(Qt::OpenHandCursor));
    //    } else {

    //        if (_currentCursorShape == Qt::OpenHandCursor) {
    //            _currentCursorShape = Qt::ArrowCursor;
    //            Q_EMIT sigSetCursor(QCursor(Qt::ArrowCursor));
    //        }
    //    }
    //    if (_areaInfo->altKeyPressed() == false) {

    //        if (_automation->fixedVertical() == false) {
    //            if ((_pointActive || _innerActive || _outerActive) && _automation->size() > 0) {
    //                drawLevel(painter);
    //            }
    //            if (_innerActive && _automation->size() == 0) {
    //                drawLevel(painter);
    //            }
    //        }
    //    }
}

Automation::HitState AutomationItem::hitPressedState() const
{
    return _hitPressedState;
}

QPointF AutomationItem::hitHoverPoint() const
{
    return _hitHoverPoint;
}

Automation::HitState AutomationItem::hitHoverState() const
{
    return _hitHoverState;
}

QPointF AutomationItem::hitPressedPoint() const
{
    return _hitPressedPoint;
}

QString AutomationItem::currentValue() const
{
    return _currentValue;
}

int AutomationItem::currentSelectedPointIndex() const
{
    return _currentSelectedPointIndex;
}

int AutomationItem::currentMovingIndex() const
{
    return _currentMovingIndex;
}

int AutomationItem::currentIndex() const
{
    return _currentIndex;
}

bool AutomationItem::mouseShapeChangingEvent() const
{
    return _mouseShapeChangingEvent;
}

void AutomationItem::setMouseShapeChangingEvent(bool newMouseShapeChangingEvent)
{
    _mouseShapeChangingEvent = newMouseShapeChangingEvent;
}

bool AutomationItem::selectAllRangeEvent() const
{
    return _selectAllRangeEvent;
}

void AutomationItem::setSelectAllRangeEvent(bool newSelectAllRangeEvent)
{
    _selectAllRangeEvent = newSelectAllRangeEvent;
}

void AutomationItem::setCurrentRangeIndex(int newCurrentRangeIndex)
{
    _currentRangeIndex = newCurrentRangeIndex;
}

void AutomationItem::setPointActive(bool newPointActive)
{
    _pointActive = newPointActive;
}

void AutomationItem::setInnerActive(bool newInnerActive)
{
    _innerActive = newInnerActive;
}

void AutomationItem::setOuterActive(bool newOuterActive)
{
    _outerActive = newOuterActive;
}

bool AutomationItem::checkStrachingActive() const
{
    return _checkStrachingActive;
}

void AutomationItem::setCheckStrachingActive(bool newCheckStrachingActive)
{
    _checkStrachingActive = newCheckStrachingActive;
}

void AutomationItem::setLastStartMarker(double newLastStartMarker)
{
    _lastStartMarker = newLastStartMarker;
}

void AutomationItem::setLastEndMarker(double newLastEndMarker)
{
    _lastEndMarker = newLastEndMarker;
}

double AutomationItem::lastEndMarker() const
{
    return _lastEndMarker;
}

double AutomationItem::lastStartMarker() const
{
    return _lastStartMarker;
}

void AutomationItem::setLiveMovingEnabled(bool newLiveMovingEnabled)
{
    _liveMovingEnabled = newLiveMovingEnabled;
}

bool AutomationItem::selectedPointsMoving() const
{
    return _selectedPointsMoving;
}

void AutomationItem::setSelectedPointsMoving(bool newSelectedPointsMoving)
{
    _selectedPointsMoving = newSelectedPointsMoving;
}

bool AutomationItem::selectedLinesMoving() const
{
    return _selectedLinesMoving;
}

void AutomationItem::setSelectedLinesMoving(bool newSelectedLinesMoving)
{
    _selectedLinesMoving = newSelectedLinesMoving;
}

bool AutomationItem::saveingLastTemplatesLines() const
{
    return _saveingLastTemplatesLines;
}

void AutomationItem::setSaveingLastTemplatesLines(bool newSaveingLastTemplatesLines)
{
    _saveingLastTemplatesLines = newSaveingLastTemplatesLines;
}

bool AutomationItem::mouseDoubleClick() const
{
    return _mouseDoubleClick;
}

bool AutomationItem::mouseClick() const
{
    return _mouseClick;
}

bool AutomationItem::mouseHover() const
{
    return _mouseHover;
}

int AutomationItem::currentMouseHoverY() const
{
    return _currentMouseHoverY;
}

int AutomationItem::currentMouseHoverX() const
{
    return _currentMouseHoverX;
}

bool AutomationItem::mousePressAndHold() const
{
    return _mousePressAndHold;
}

int AutomationItem::currentRangeIndex() const
{
    return _currentRangeIndex;
}

bool AutomationItem::pointActive() const
{
    return _pointActive;
}

bool AutomationItem::innerActive() const
{
    return _innerActive;
}

bool AutomationItem::outerActive() const
{
    return _outerActive;
}

void AutomationItem::setCurrentValue(const QString& newCurrentValue)
{
    _currentValue = newCurrentValue;
}

int AutomationItem::liveMovingEnabled()
{
    return _liveMovingEnabled;
}

int AutomationItem::trackIndex() const
{
    return _trackIndex;
}

void AutomationItem::setEnabled(bool enabled)
{

    bool isEnabled = _automation->enabled();

    if (isEnabled != enabled) {
        _automation->setEnabled1(enabled);

        QJsonArray array;
        QJsonObject jObject2;
        jObject2["track_index"] = _trackIndex;
        jObject2["sub_track_index"] = _automation->getSubTrackIndex();
        jObject2["menu"] = _automation->type();
        jObject2["sub_menu"] = _automation->subType();

        if (enabled)
            jObject2["operation"] = ActionStack::Operation_EnableAutomationPoint;
        else
            jObject2["operation"] = ActionStack::Operation_DisableAutomationPoint;

        array.append(jObject2);
        ActionStack::instance()->pushInUndo(array);
    }
}

QSharedPointer<Automation> AutomationItem::automation() const
{
    return _automation;
}

void AutomationItem::holdTriggered()
{

    if (_mouseRelease == false && _mouseDoubleClick == false && _mousePress == true && _mousePressAndHold == false) {

        _mousePressAndHold = true;
        _automation->createTemp();

        auto onThreshold = ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor());
        auto nearThreshold = ScreenInterface::scaleSize2(40, _areaInfo->zoomFactor());
        if (!_automation->isAutomated()) {
            onThreshold = ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor());
            nearThreshold = ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor());
        }
        auto hit2 = _automation->hitTestLines(_currentMouseX, _currentMouseY, onThreshold, nearThreshold);

        if (hit2.state == Automation::OnLine) {

            QPointF mapped = MathHelper1::mapPointToLine(hit2.index.fp1(), hit2.index.fp2(), QPointF(_currentMouseX, _currentMouseY));
            if (hit2.index.p1()->curve_n() != 1.0) {

                mapped = MathHelper1::mapPointToCurve(hit2.index.fp1(), hit2.index.fp2(), QPointF(_currentMouseX, _currentMouseY), hit2.index.p1()->curve_n(), hit2.index.p1()->curve_m());
            }

            saveInUndoItems();

            auto cPoint = _automation->add_point(_automation->pixel2Time(mapped.x()), _automation->pixel2Y(mapped.y()));

            _automation->setSelectedPointIndex(cPoint);

            _lastMouseMovingPoint = mapped.toPoint();

            _timerCursorPosition.start();
        }
        if (hit2.state == Automation::OnPoint) {

            saveInUndoItems();

            _lastMouseMovingPoint = QPoint(_mousePressX, _mousePressY);
            _timerCursorPosition.start();

            _automation->setSelectedPointIndex(hit2.index.p2());
        }

        if (hit2.state == Automation::NearLine) {

            _timerCursorPosition.start();

            saveInUndoItems();

            if (_automation->hoveredLinesCount() == 0 && _automation->selectedLines()->size() == 0) {
                _automation->setSelectedLine(hit2.index.p1(), hit2.index.p2());

                if (_automation->selectedLine()->head()->isNegInf()) {
                    _diffPressed = QSizeF(_currentMouseX - _automation->time2Pixel(_automation->selectedLine()->head()->next()->time()),
                        _currentMouseY - _automation->getYPixel(_automation->selectedLine()->head()->next()->y()));
                } else {
                    _diffPressed = QSizeF(_currentMouseX - _automation->time2Pixel(_automation->selectedLine()->head()->time()),
                        _currentMouseY - _automation->getYPixel(_automation->selectedLine()->head()->y()));
                }
            }

            if (_automation->hoveredLinesCount() > 0 && _automation->selectedLines()->size() == 0 && _mousePressAndHold && _automation->selectedLineHovered()) {

                _automation->createSelectedLines();

                if (_automation->selectedLines()->head()->isNegInf()) {

                    _diffPressed = QSizeF(_currentMouseX - _automation->time2Pixel(_automation->selectedLines()->head()->next()->time()),
                        _currentMouseY - _automation->getYPixel(_automation->selectedLines()->head()->next()->y()));
                } else {
                    _diffPressed = QSizeF(_currentMouseX - _automation->time2Pixel(_automation->selectedLines()->head()->time()),
                        _currentMouseY - _automation->getYPixel(_automation->selectedLines()->head()->y()));
                }
            }

            _minYInSelectedcLines = _automation->getWindowRect().top() - (_automation->getMaxYinSelectedPoints() - _mousePressY);
            _maxYInSelectedcLines = _automation->getWindowRect().bottom() + (_mousePressY - _automation->getMinYinSelectedPoints());
        }

        _currentCursorShape = Qt::BlankCursor;
        Q_EMIT sigSetCursor(QCursor(Qt::BlankCursor));

        Q_EMIT sigUpdate();
    }
}

void AutomationItem::sltTimerMouse()
{

    holdTriggered();
    _timerCheckPressAndHold.stop();
}

void AutomationItem::sltTimerDoubleClicked()
{
    if (_mouseDoubleClickCounter >= 2) {

        _mouseDoubleClick = true;

        Q_EMIT sigUpdate();
    }
    _mouseDoubleClickCounter = 0;
    _timerCheckDoubleClick.stop();
}

int AutomationItem::mousePressEvent(QMouseEvent* event)
{

    if (_showEnvelopes == false) {
        return AutomationItemEvent_NotSelect;
    }

    if (!_rect.contains(event->pos()))
        return AutomationItemEvent_NotSelect;

    _moveDirectionLeft = MoveDirection_None;
    _saveingLastTemplatesLines = true;

    _currentMouseX = event->pos().x();
    _currentMouseY = event->pos().y();

    _mousePressX = event->pos().x();
    _mousePressY = event->pos().y();

    _preMouseX = event->pos().x();
    _preMouseY = event->pos().y();

    if (_automation->fixedVertical() == true) {
        _currentMouseY = _mousePressY = _preMouseY = _automation->getYPixel(_automation->defaultNormalized());
    }

    if (_timerCheckDoubleClick.isActive() == false) {
        _mouseDoubleClickCounter = 0;
        _timerCheckDoubleClick.start();
    }

    auto onThreshold = ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor());
    auto nearThreshold = ScreenInterface::scaleSize2(40, _areaInfo->zoomFactor());

    auto hit2 = _automation->hitTestLines(_currentMouseX, _currentMouseY, onThreshold, nearThreshold);

    _automation->setSelectedPointIndex(nullptr);
    _automation->selectedLine()->clear();
    // _automation->clearHoveredLines();

    _mousePressAndHold = false;

    _mouseRelease = false;
    _mousePress = false;
    _timerCheckPressAndHold.stop();
    Q_EMIT sigUpdate();

    int T = _rect.top();
    int B = _rect.bottom();
    _maxYInSelectedcLines = B;
    _minYInSelectedcLines = T;

    if (hit2.state == Automation::OnPoint) {

        _mousePress = true;
        _timerCheckPressAndHold.start();
        return AutomationItemEvent_InnerSelect;
    } else if (hit2.state == Automation::OnLine) {

        _mousePress = true;
        _timerCheckPressAndHold.start();
        return AutomationItemEvent_InnerSelect;
    } else if (hit2.state == Automation::NearLine) {

        _mousePress = true;
        _timerCheckPressAndHold.start();
        return AutomationItemEvent_OuterPointSelect;
    }

    return AutomationItemEvent_Select;
}

int AutomationItem::mouseMoveEvent(QMouseEvent* event)
{

    // There is a problem for fast moving here, if user moves mouse fast, _currentMouseY won't apply.
    // to fix that, I think checkevent should follow mouseMoveEvent to be dont after that
    if (_showEnvelopes == false) {
        return 0;
    }

    //    if (!_rect.contains(event->pos()))
    //        return AutomationItemEvent_NotSelect;

    int L = _rect.left();
    int R = _rect.right();

    int T = _rect.top();
    int B = _rect.bottom();

    if (_mousePress && _mousePressAndHold == false) {

        holdTriggered();
    } else {
    }

    _diff = QSizeF(event->pos().x() - _preMouseX, event->pos().y() - _preMouseY);

    // This part is for when we move mouse in left size and x wil be negetive but we move mouse right side it should start from zero
    if ((_moveDirectionLeft == MoveDirection_Left || _moveDirectionLeft == MoveDirection_None) && _preMouseX < 0) {
        if (_diff.width() > 0 && _currentMouseX < 0) {
            _currentMouseX = 0;
        }
    }
    if (_diff.width() < 0) {
        _moveDirectionLeft = MoveDirection_Left;
    } else if (_diff.width() > 0) {
        _moveDirectionLeft = MoveDirection_Right;
    } else {
        _moveDirectionLeft = MoveDirection_None;
    }

    _preMouseX = event->pos().x();
    _preMouseY = event->pos().y();

    if (_mousePressAndHold) {

        if (_automation->fixedVertical() == true) {
            _currentMouseX += _diff.width();
        } else {
            if (_areaInfo->shiftkeyPressed()) {

                float H = _rect.bottom() - _rect.top();

                auto coeff = qAbs(_diff.height()) / 2.0;
                double p = H / 1000.0;
                if (coeff > 0)
                    p *= coeff;

                if (_diff.height() > 0) {
                    _currentMouseY += p;
                }
                if (_diff.height() < 0) {
                    _currentMouseY -= p;
                }

            } else if (_areaInfo->ctrlkeyPressed()) {
                _currentMouseX += _diff.width();
            } else {
                _currentMouseX += _diff.width();
                _currentMouseY += _diff.height();
            }
        }

        if (_currentMouseY < _minYInSelectedcLines) {
            _currentMouseY = _minYInSelectedcLines;
        }
        if (_currentMouseY > _maxYInSelectedcLines) {
            _currentMouseY = _maxYInSelectedcLines;
        }

        if (_automation->discreted()) {

            if (qAbs(_currentMouseY - _minYInSelectedcLines) < (B - T) / 4.0) {
                if (qAbs(_currentMouseY - _minYInSelectedcLines) > 1) {
                    _currentMouseY = _maxYInSelectedcLines;
                } else {
                    _currentMouseY = _minYInSelectedcLines;
                }
            }

            if (qAbs(_currentMouseY - _maxYInSelectedcLines) < (B - T) / 4.0) {
                if (qAbs(_currentMouseY - _maxYInSelectedcLines) > 1) {
                    _currentMouseY = _minYInSelectedcLines;
                } else {
                    _currentMouseY = _maxYInSelectedcLines;
                }
            }

            if (_currentMouseY <= _minYInSelectedcLines) {
                _currentMouseY = _minYInSelectedcLines;
            }

            if (_currentMouseY >= _maxYInSelectedcLines) {
                _currentMouseY = _maxYInSelectedcLines;
            }
        }

        if (_automation->isAutomated()) {
            setAutomatedNormalized(automation()->getY(_currentIndicator));

            if (automation()->type() == "Mixer") {

                double final_xpos = _currentMouseX;
                double final_ypos = _currentMouseY;

                _automation->findSnap(_currentMouseX, _currentMouseY, final_xpos, final_ypos);

            } else {
            }
        }

        if (_areaInfo->altKeyPressed() == false)
            _lastMouseMovingPoint = QPoint(_currentMouseX, _currentMouseY);

        // Q_EMIT sigCursorPosition(QPoint(_mousePressX, _mousePressY));

        _currentCursorShape = Qt::BlankCursor;
        // Q_EMIT sigSetCursor(QCursor(Qt::BlankCursor));

        _mouseMoving = true;
    }

    _hitHoverState = Automation::None;

    if (_automation->selectedPointIndex() && _mousePressAndHold) {

        double final_xpos = _currentMouseX;
        double final_ypos = _currentMouseY;

        _automation->createSnapLine(true, _hitPressedPoint);

        _automation->findSnap(_currentMouseX, _currentMouseY, final_xpos, final_ypos);

        if (final_ypos < _automation->getWindowRect().top()) {
            final_ypos = _automation->getWindowRect().top();
        } else if (final_ypos > _automation->getWindowRect().bottom()) {
            final_ypos = _automation->getWindowRect().bottom();
        } else {
            final_ypos = _currentMouseY;
        }

        _hitPressedPoint = QPointF(final_xpos, final_ypos);

        _automation->edit_point(_automation->selectedPointIndex(), _automation->pixel2Time(final_xpos), _automation->pixel2Y(final_ypos));

        _currentValue = normalized2Label(_automation->pixel2Y(final_ypos));
        setAutomatedNormalized(automation()->getY(_currentIndicator));

        _hitPressedState = Automation::OnPoint;

        _areaInfo->setEditPointIndicator(_automation->pixel2Time(final_xpos));

        Q_EMIT sigUpdate();
    }

    if (_automation->selectedLine()->size() > 0 && _mousePressAndHold) {

        if (_areaInfo->altKeyPressed()) {

            float Y1 = _automation->getYPixel(_automation->selectedLine()->head()->y());
            float Y2 = _automation->getYPixel(_automation->selectedLine()->tail()->y());

            if (Y1 < Y2) {
                float mid = qAbs(Y2 - Y1) / 2.0 + Y1;

                float curve_n = _automation->selectedLine()->head()->curve_n();
                float curve_m = _automation->selectedLine()->head()->curve_m();

                if (_currentMouseY < mid) {
                    curve_n = 9.0 / (Y1 - mid) * (_currentMouseY - mid) + 1;
                    curve_m = 9.0 / (Y1 - mid) * (_currentMouseY - mid) + 1;

                } else if (_currentMouseY > mid) {
                    curve_n = -9.0 / (Y2 - mid) * (_currentMouseY - mid) - 1;
                    curve_m = -9.0 / (Y2 - mid) * (_currentMouseY - mid) - 1;

                } else {
                    curve_n = 1;
                    curve_m = 1;
                }

                if (curve_n <= -4.5) {
                    curve_n = -4.5;
                }
                if (curve_m <= -4.5) {
                    curve_m = -4.5;
                }
                if (curve_n > 4.5)
                    curve_n = 4.5;

                if (curve_m > 4.5)
                    curve_m = 4.5;

                _automation->selectedLine()->head()->setCurve_n(curve_n);
                _automation->selectedLine()->head()->setCurve_m(curve_m);

            } else {

                float mid = qAbs(Y2 - Y1) / 2.0 + Y2;
                float curve_n = _automation->selectedLine()->head()->curve_n();
                float curve_m = _automation->selectedLine()->head()->curve_m();

                if (_currentMouseY < mid) {
                    curve_n = -9.0 / (Y2 - mid) * (_currentMouseY - mid) - 1;
                    curve_m = -9.0 / (Y2 - mid) * (_currentMouseY - mid) - 1;
                } else if (_currentMouseY > mid) {
                    curve_n = 9.0 / (Y1 - mid) * (_currentMouseY - mid) + 1;
                    curve_m = 9.0 / (Y1 - mid) * (_currentMouseY - mid) + 1;
                } else {
                    curve_n = 1;
                    curve_m = 1;
                }

                if (curve_n <= -4.5) {
                    curve_n = -4.5;
                }
                if (curve_m <= -4.5) {
                    curve_m = -4.5;
                }
                if (curve_n > 4.5)
                    curve_n = 4.5;

                if (curve_m > 4.5)
                    curve_m = 4.5;

                _automation->selectedLine()->head()->setCurve_n(curve_n);
                _automation->selectedLine()->head()->setCurve_m(curve_m);
            }
        } else {
            double final_xpos = _currentMouseX - _diffPressed.width();
            double final_ypos = _currentMouseY - _diffPressed.height();

            _automation->createSnapLine(true);

            _automation->findSnap(_currentMouseX - _diffPressed.width(), _currentMouseY - _diffPressed.height(), final_xpos, final_ypos);

            if (_automation->selectedLine()->tail()->isPosInf()) {
                _hitPressedPoint = _automation->cPoint2QPoint(_automation->selectedLine()->head());
            } else if (_automation->selectedLine()->head()->isNegInf()) {
                _hitPressedPoint = _automation->cPoint2QPoint(_automation->selectedLine()->tail());
            } else {

                double duration = (_automation->time2Pixel(_automation->selectedLine()->tail()->time()) - _automation->time2Pixel(_automation->selectedLine()->head()->time()));

                if (_diffPressed.width() < duration / 2) {
                    _hitPressedPoint = _automation->cPoint2QPoint(_automation->selectedLine()->head());

                    _currentValue = normalized2Label(_automation->selectedLine()->head()->y());

                } else {

                    _hitPressedPoint = _automation->cPoint2QPoint(_automation->selectedLine()->tail());

                    _currentValue = normalized2Label(_automation->selectedLine()->tail()->y());
                }
            }

            QPointF newPoint = QPointF(_automation->pixel2Time(final_xpos), final_ypos);

            double dt = newPoint.x() - _automation->selectedLine()->head()->time();
            if (_automation->selectedLine()->head()->isNegInf()) {
                dt = newPoint.x() - _automation->selectedLine()->head()->next()->time();
            }

            double dy = newPoint.y() - _automation->getYPixel(_automation->selectedLine()->head()->y());

            _hitPressedState = Automation::NearLine;

            _automation->edit_line(_automation->selectedLine(), QPointF(dt, dy));
            setAutomatedNormalized(automation()->getY(_currentIndicator));

            _areaInfo->setEditPointIndicator(_automation->pixel2Time(final_xpos));

            Q_EMIT sigUpdate();
        }
    }

    if (_automation->selectedLines()->size() > 0 && _mousePressAndHold) {

        double final_xpos = _currentMouseX - _diffPressed.width();
        double final_ypos = _currentMouseY - _diffPressed.height();

        _automation->createSnapLine(true);

        _automation->findSnap(_currentMouseX - _diffPressed.width(), _currentMouseY - _diffPressed.height(), final_xpos, final_ypos);

        QPointF newPoint = QPointF(_automation->pixel2Time(final_xpos), final_ypos);
        double dt = newPoint.x() - _automation->selectedLines()->head()->time();

        double duration = _automation->time2Pixel(_automation->selectedLines()->tail()->time()) - _automation->time2Pixel(_automation->selectedLines()->head()->time());

        if (_diffPressed.width() > duration / 2) {
            _hitPressedPoint = _automation->cPoint2QPoint(_automation->selectedLines()->tail()->prev());

            _currentValue = normalized2Label(_automation->selectedLines()->tail()->prev()->y());

        } else {
            _hitPressedPoint = _automation->cPoint2QPoint(_automation->selectedLines()->head()->next());

            _currentValue = normalized2Label(_automation->selectedLines()->head()->next()->y());
        }

        if (_automation->selectedLines()->head()->isNegInf()) {
            dt = newPoint.x() - _automation->selectedLines()->head()->next()->time();
        }

        double dy = newPoint.y() - _automation->getYPixel(_automation->selectedLines()->head()->next()->y());

        _automation->edit_line(_automation->selectedLines(), QPointF(dt, dy));

        _hitPressedState = Automation::NearLine;

        setAutomatedNormalized(automation()->getY(_currentIndicator));

        _areaInfo->setEditPointIndicator(_automation->pixel2Time(final_xpos));

        Q_EMIT sigUpdate();
    }

    // checkEvents();

    return 0;
}

int AutomationItem::mouseReleaseEvent(QMouseEvent* event)
{

    if (_showEnvelopes == false) {
        return 0;
    }

    _areaInfo->setEditPointIndicator(-1);

    _timerCursorPosition.stop();
    _lastMouseMovingPoint = event->pos();

    if (_autoScrollLeft) {
        _autoScrollLeft = false;
    }
    if (_autoScrollRight) {
        _autoScrollRight = false;
    }

    _mouseDoubleClickCounter++;

    _timerCheckPressAndHold.stop();

    if (_mousePressAndHold == false && _mousePress == true) {
        _mouseClick = true;
    }

    if (_mousePressAndHold && (_liveMovingEnabled || _selectedLinesMoving || _selectedPointsMoving)) {
        _saveingLastTemplatesLines = true;
    }

    if (_mousePressAndHold == false && _mousePress == true) {
        auto onThreshold = ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor());
        auto nearThreshold = ScreenInterface::scaleSize2(40, _areaInfo->zoomFactor());

        auto hit2 = _automation->hitTestLines(_currentMouseX, _currentMouseY, onThreshold, nearThreshold);

        if (hit2.state == Automation::OnPoint) {
            // For lanes where left-click delete is disabled (KeyFrames), a
            // stationary click on a point should select it, not delete it.
            // Delete is reachable via the right-click context menu.
            if (_clickDeleteEnabled) {
                saveInUndoItems();
                _automation->delete_point(hit2.index.p2());
            }
        }
        if (hit2.state == Automation::OnLine) {
            saveInUndoItems();
            QPointF mapped = MathHelper1::mapPointToLine(hit2.index.fp1(), hit2.index.fp2(), QPointF(_currentMouseX, _currentMouseY));

            if (hit2.index.p1()->curve_n() != 1.0) {

                mapped = MathHelper1::mapPointToCurve(hit2.index.fp1(), hit2.index.fp2(), QPointF(_currentMouseX, _currentMouseY), hit2.index.p1()->curve_n(), hit2.index.p1()->curve_m());
            }

            _automation->add_point(_automation->pixel2Time(mapped.x()), _automation->pixel2Y(mapped.y()));
        }
    } else {
    }

    if (_mousePressAndHold && _hitPressedState != Automation::None) {

        int new_x = _currentMouseX + _startDiffX, new_y = _currentMouseY;
        if (_automation->fixedVertical() == true) {
            if (_currentMouseX > width) {
                new_x = _rect.width();
            } else if (_currentMouseX < 0) {
                new_x = 0;
            }
            _currentMouseY = new_y = _automation->getYPixel(_automation->defaultNormalized());
        } else {
            if (_currentMouseX > width) {
                new_x = _rect.width();
            } else if (_currentMouseY < _rect.top()) {
                new_y = _rect.top();
            } else if (_currentMouseY > _rect.bottom()) {
                new_y = _rect.bottom();
            } else if (_currentMouseX < 0) {
                new_x = 0;
            }
        }

        _startDiffX = 0;
        _startDiffY = 0;
        _hitPressedState = Automation::None;

        _mousePressAndHold = false;
        _mousePress = false;
        _automation->clearHovered();
        _automation->saveTemp();

        _automation->setSelectedPointIndex(nullptr);
        _automation->selectedLine()->clear();

        _automation->clearHoveredLines();

        Q_EMIT sigCursorPosition(QPoint(new_x, new_y));
    }

    _startDiffX = 0;
    _startDiffY = 0;
    _hitPressedState = Automation::None;

    _mousePressAndHold = false;

    Q_EMIT sigUpdate();

    automation()->saveTemplateLine();
    automation()->createSelectedPoints(_areaInfo->selectionArea()->area().left(), _areaInfo->selectionArea()->area().right());
    _automation->clearMovingLine();
    _automation->clearSelectedLines();
    _currentCursorShape = Qt::ArrowCursor;

    _startDiffX = 0;
    _startDiffY = 0;
    _hitPressedState = Automation::None;
    _mousePressAndHold = false;
    _mousePress = false;
    _automation->clearHovered();
    _automation->saveTemp();

    _automation->setSelectedPointIndex(nullptr);
    _automation->selectedLine()->clear();
    _automation->clearHoveredLines();
    _automation->clearHovered();
    _automation->saveTemp();
    _mousePress = false;
    _mouseRelease = true;
    _saveEditState = true;

    _currentIndex = -1;
    _mouseMoving = false;

    Q_EMIT sigSetCursor(QCursor(Qt::ArrowCursor));

    return 1;
}

int AutomationItem::hoverMoveEvent(QHoverEvent* event)
{

    _mouseHover = false;

    if (_showEnvelopes == false) {
        return 0;
    }

    if (_mousePressAndHold) {
        return 0;
    }

    _currentMouseHoverX = event->pos().x();
    _currentMouseHoverY = event->pos().y();

    if (!_rect.contains(event->pos())) {

        Q_EMIT sigUpdate();
        _mouseHover = false;
        _hitHoverState = Automation::None;
        _hitPressedState = Automation::None;
        return 0;
    }
    _mouseHover = true;

    auto selectedArea = _areaInfo->selectionArea();
    bool selectedHovred = false;
    double eps = 1e-9;

    _automation->clearHoveredLines();
    if ((_trackIndex > selectedArea->firstTrackIndex() && _trackIndex < selectedArea->lastTrackIndex())
        || (_trackIndex == selectedArea->firstTrackIndex() && selectedArea->firstTrackIndex() != selectedArea->lastTrackIndex() && automation()->getSubTrackIndex() >= selectedArea->_firstSubTrackIndex)
        || (_trackIndex == selectedArea->lastTrackIndex() && selectedArea->firstTrackIndex() != selectedArea->lastTrackIndex() && automation()->getSubTrackIndex() <= selectedArea->_lastSubTrackIndex)
        || (selectedArea->firstTrackIndex() == _trackIndex && selectedArea->firstTrackIndex() == selectedArea->lastTrackIndex() && automation()->getSubTrackIndex() >= selectedArea->_firstSubTrackIndex
            && automation()->getSubTrackIndex() <= selectedArea->_lastSubTrackIndex)) {

        if (_automation->pixel2Time(_currentMouseHoverX) > selectedArea->area().left() && _automation->pixel2Time(_currentMouseHoverX) < selectedArea->area().right()) {

            selectedHovred = true;

            if (_automation->selectedLines()->size() == 0) {
                _automation->createHoveredLines(selectedArea->area().left(), selectedArea->area().right());
            }
        }
    }

    auto onThreshold = ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor());
    auto nearThreshold = ScreenInterface::scaleSize2(40, _areaInfo->zoomFactor());

    if (!_automation->isAutomated()) {
        onThreshold = ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor());
        nearThreshold = ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor());
    }

    auto hit2 = _automation->hitTestLines(_currentMouseHoverX, _currentMouseHoverY, onThreshold, nearThreshold);
    _hitHoverPoint = hit2.mapped;
    _hitHoverState = hit2.state;

    _automation->clearHovered();
    _automation->setSelectedLineHovered(false);
    if (hit2.state == Automation::OnLine) {

        _automation->setDashedLineHovred(hit2.mapped);

    } else {

        if (hit2.state == Automation::NearLine) {

            if (_automation->hoveredLinesCount() > 0 && selectedHovred) {
                _automation->setDashedLineHovred(QPointF());
                _automation->setSelectedLineHovered(true);

                double mid = (_automation->time2Pixel(_automation->hoveredLines()->tail()->time()) + _automation->time2Pixel(_automation->hoveredLines()->head()->time())) / 2.0;

                if (_currentMouseHoverX < mid) {
                    _hitHoverPoint = _automation->cPoint2QPoint(_automation->hoveredLines()->head());

                } else {
                    _hitHoverPoint = _automation->cPoint2QPoint(_automation->hoveredLines()->tail());
                }

            } else {

                double mid = (_automation->time2Pixel(hit2.index.p1()->time()) + _automation->time2Pixel(hit2.index.p2()->time())) / 2.0;

                if (hit2.index.p2()->isPosInf()) {
                    _hitHoverPoint = _automation->cPoint2QPoint(hit2.index.p1());
                } else if (hit2.index.p1()->isNegInf()) {
                    _hitHoverPoint = _automation->cPoint2QPoint(hit2.index.p2());
                } else {

                    if (_currentMouseHoverX < mid) {
                        _hitHoverPoint = _automation->cPoint2QPoint(hit2.index.p1());

                    } else {
                        _hitHoverPoint = _automation->cPoint2QPoint(hit2.index.p2());
                    }
                }

                _automation->setLineHovered(&hit2.index, true);
                _automation->setDashedLineHovred(hit2.mapped);
            }
        } else {

            _automation->setDashedLineHovred(QPointF());
        }
    }

    if (automation()->type() == "Mixer") {
        _currentValue = normalized2Label(_automation->pixel2Y(hit2.mapped.y()));
    }

    _areaInfo->setCurrentHoveTrackIndex(_trackIndex);

    Q_EMIT sigUpdate();
    // checkEvents();
    return hit2.state == Automation::None ? 0 : 1;
}

int AutomationItem::mouseDoubleClickEvent(QMouseEvent* event)
{

    return false;
}

int AutomationItem::keyPressEvent(int modifier, int key)
{

    if (_showEnvelopes == false) {
        return 0;
    }

    if (modifier == Qt::ControlModifier && key == Qt::Key_C) {
        return automation()->contains(*_areaInfo->selectionArea());
    }
    if (modifier == Qt::ControlModifier && key == Qt::Key_V) {
        return insertCopiedList();
    }

    if (modifier == Qt::ControlModifier && key == Qt::Key_X) {

        return cutCopiedList(*_areaInfo->selectionArea());
    }

    if (modifier == Qt::ControlModifier && key == Qt::Key_A) {
        _selectAllRangeEvent = true;
    }
    //    if (modifier == Qt::ShiftModifier) {

    //        _automation->setLimitYMoving(false);
    //        _automation->setLimitXMoving(true);
    //    }

    //    if (modifier == Qt::ControlModifier) {

    //        _automation->setLimitYMoving(true);
    //        _automation->setLimitXMoving(false);
    //    }

    //    if (modifier == Qt::AltModifier) {

    //        _automation->setLimitYMoving(true);
    //        _automation->setLimitXMoving(false);
    //    }

    if (key == Qt::Key_Delete) {

        _automation->deleteLines(_areaInfo->selectionArea()->area().left(), _areaInfo->selectionArea()->area().right());
    }

    return 0;
}

int AutomationItem::keyReleaseEvent(int modifier, int key)
{

    if (_showEnvelopes == false) {
        return 0;
    }

    //    _automation->setLimitYMoving(false);
    //    _automation->setLimitXMoving(false);
    _currentCursorShape = Qt::ArrowCursor;
    Q_EMIT sigSetCursor(QCursor(Qt::ArrowCursor));
    return 1;
}

bool AutomationItem::undo()
{
    return true;
}

bool AutomationItem::redo()
{
    return true;
}

void AutomationItem::sltTimerMoving()
{
    if (_mousePressAndHold) {
        if (_currentMouseX > _rect.width()) {
            _diff.setWidth(_currentMouseX - rect().width());
        }
    }
}

bool AutomationItem::isSelectedLines()
{

    //    auto innerThreshold = ScreenInterface::scaleSize2(30, _areaInfo->zoomFactor());
    //    auto outterThreshold = ScreenInterface::scaleSize2(60, _areaInfo->zoomFactor());

    //    auto ranges = _automation->selectedLines2Ranegs();

    //    for (int i = 0; i < ranges.count(); i++) {

    //        float curve_m = 1;
    //        float curve_n = 1;

    //        bool valid = _automation->getCurve(i, curve_n, curve_m, _liveMovingEnabled);

    //        if (curve_m != 1.0 || curve_n != 1.0) {

    //            bool contains = MathHelper1::isInSuperEllipseArea(ranges[i].p2(), ranges[i].p1(), curve_n, curve_m, _currentMouseHoverX, _currentMouseHoverY, outterThreshold);

    //            bool p = MathHelper1::superEllipseContains(ranges[i].p2().toPoint(), ranges[i].p1().toPoint(), curve_n, curve_m, _currentMouseHoverX, _currentMouseHoverY);

    //            if (_outerActive) {
    //                if (contains) {
    //                    if (p == false)
    //                        return true;
    //                }
    //            }

    //        } else {

    //            QPolygonF _polyInner;
    //            QPolygonF _polyOuter;

    //            MathHelper1::lineAreas(ranges[i].p1(), ranges[i].p2(), _polyInner, _polyOuter, innerThreshold, outterThreshold);

    //            if (_outerActive) {
    //                if (qAbs(ranges[i].p2().x() - ranges[i].p1().x()) > 0) {

    //                    if (_polyInner.containsPoint(QPointF(_currentMouseHoverX, _currentMouseHoverY), Qt::WindingFill)) {

    //                    } else if (_polyOuter.containsPoint(QPointF(_currentMouseHoverX, _currentMouseHoverY), Qt::WindingFill)) {

    //                        return true;
    //                    }

    //                } else {

    //                    if (_polyOuter.containsPoint(QPointF(_currentMouseHoverX, _currentMouseHoverY), Qt::WindingFill)) {

    //                        return true;
    //                    }
    //                }
    //            }
    //        }
    //    }
    return false;
}

bool AutomationItem::createCopyList(SelectedArea selectedArea)
{

    if (isInsideSelectedArea(selectedArea)) {
        if (_automation->createCopyLines(selectedArea.area().left(), selectedArea.area().right())) {
            return true;
        }
    }
    return false;
}

bool AutomationItem::insertCopiedList()
{

    auto selectedArea = _areaInfo->selectionArea();

    int index = 0;
    if (selectedArea->_firstSubTrackIndex == selectedArea->_lastTrackIndex) {
        index = selectedArea->_firstSubTrackIndex + 1;
    }

    _automation->insertLines((*selectedArea).area().left(), (*selectedArea).area().right(), index);
    Q_EMIT sigUpdate();
    return true;

    return false;
}

bool AutomationItem::cutCopiedList(SelectedArea selectedArea)
{

    if (isInsideSelectedArea(selectedArea)) {

        if (_automation->createCopyLines(selectedArea.area().left(), selectedArea.area().right())) {
            _automation->deleteLines(selectedArea.area().left(), selectedArea.area().right());
            Q_EMIT sigUpdate();
            return true;
        }
    }
    return false;
}

bool AutomationItem::removedCopiedList(SelectedArea selectedArea)
{

    if (isInsideSelectedArea(selectedArea)) {
        _automation->deleteLines(selectedArea.area().left(), selectedArea.area().right());
        Q_EMIT sigUpdate();
        return true;
    }

    return false;
}

void AutomationItem::setLinesState(QJsonArray states)
{
    _automation->setLinesState(states);
    Q_EMIT sigUpdate();
}

QJsonArray AutomationItem::getLinesState()
{
    return _automation->getLinesState();
}

void AutomationItem::setCurrentIndicator(double indicator)
{

    _currentIndicator = indicator;
}

void AutomationItem::sltTimerCheckPosition()
{
    _autoScrollRight = false;
    _autoScrollLeft = false;

    float _sutoScrollSpeedRight = qAbs(rect().width() - _lastMouseMovingPoint.x()) / 20.0;
    float _sutoScrollSpeedLeft = qAbs(_lastMouseMovingPoint.x()) / 20.0;

    float startTime = _areaInfo->startTime();
    float endTime = _areaInfo->endTime();

    if (_lastMouseMovingPoint.x() > rect().width()) {

        _autoScrollRight = true;

        float rate = _sutoScrollSpeedRight * (_areaInfo->clipDuration() / (double)_areaInfo->lastWidth());

        _areaInfo->setClipProperties(startTime + rate, endTime + rate, endTime - startTime);

        if (_areaInfo->endTime() > _areaInfo->maxEndTime()) {
            _areaInfo->setMaxEndTime(_areaInfo->endTime());
        }

        if (_automation->selectedPointIndex() && _mousePressAndHold) {

            _automation->edit_point(_automation->selectedPointIndex(), _automation->pixel2Time(rect().width()), _automation->pixel2Y(_currentMouseY));
        }

        if (_automation->selectedLine()->size() > 0 && _mousePressAndHold) {

            double final_xpos = rect().width() - _diffPressed.width();
            double final_ypos = _currentMouseY - _diffPressed.height();

            _automation->createSnapLine(true);

            _automation->findSnap(_currentMouseX - _diffPressed.width(), _currentMouseY - _diffPressed.height(), final_xpos, final_ypos);

            QPointF newPoint = QPointF(_automation->pixel2Time(final_xpos), final_ypos);

            double dt = newPoint.x() - _automation->selectedLine()->head()->time();
            if (_automation->selectedLine()->head()->isNegInf()) {
                dt = newPoint.x() - _automation->selectedLine()->head()->next()->time();
            }

            double dy = newPoint.y() - _automation->getYPixel(_automation->selectedLine()->head()->y());

            _automation->edit_line(_automation->selectedLine(), QPointF(dt, dy));

            _currentValue = normalized2Label(_automation->selectedLine()->head()->y());
            setAutomatedNormalized(automation()->getY(_currentIndicator));
        }

        if (_automation->selectedLines()->size() > 0 && _mousePressAndHold) {

            double final_xpos = rect().width() - _diffPressed.width();
            double final_ypos = _currentMouseY - _diffPressed.height();

            _automation->createSnapLine(true);

            _automation->findSnap(_currentMouseX - _diffPressed.width(), _currentMouseY - _diffPressed.height(), final_xpos, final_ypos);

            QPointF newPoint = QPointF(_automation->pixel2Time(final_xpos), final_ypos);
            double dt = newPoint.x() - _automation->selectedLines()->head()->time();

            if (_automation->selectedLines()->head()->isNegInf()) {
                dt = newPoint.x() - _automation->selectedLines()->head()->next()->time();
            }

            double dy = newPoint.y() - _automation->getYPixel(_automation->selectedLines()->head()->next()->y());

            _automation->edit_line(_automation->selectedLines(), QPointF(dt, dy));

            _currentValue = normalized2Label(_automation->selectedLines()->head()->next()->y());
            setAutomatedNormalized(automation()->getY(_currentIndicator));
        }

        Q_EMIT sigUpdate();
    }

    if (_lastMouseMovingPoint.x() < 0 && startTime > 0) {
        _autoScrollLeft = true;

        float rate = _sutoScrollSpeedLeft * (_areaInfo->clipDuration() / (double)_areaInfo->lastWidth());

        if (startTime - rate < 0) {

            _areaInfo->setClipProperties(0, endTime - startTime, endTime - startTime);

        } else {
            _areaInfo->setClipProperties(startTime - rate, endTime - rate, endTime - startTime);
        }

        if (_automation->selectedPointIndex() && _mousePressAndHold) {

            _automation->edit_point(_automation->selectedPointIndex(), _automation->pixel2Time(0), _automation->pixel2Y(_currentMouseY));
        }

        if (_automation->selectedLine()->size() > 0 && _mousePressAndHold) {

            double final_xpos = 0 - _diffPressed.width();
            double final_ypos = _currentMouseY - _diffPressed.height();

            _automation->createSnapLine(true);

            _automation->findSnap(_currentMouseX - _diffPressed.width(), _currentMouseY - _diffPressed.height(), final_xpos, final_ypos);

            QPointF newPoint = QPointF(_automation->pixel2Time(final_xpos), final_ypos);

            double dt = newPoint.x() - _automation->selectedLine()->head()->time();
            if (_automation->selectedLine()->head()->isNegInf()) {
                dt = newPoint.x() - _automation->selectedLine()->head()->next()->time();
            }

            double dy = newPoint.y() - _automation->getYPixel(_automation->selectedLine()->head()->y());

            _automation->edit_line(_automation->selectedLine(), QPointF(dt, dy));

            _currentValue = normalized2Label(_automation->selectedLine()->head()->y());
            setAutomatedNormalized(automation()->getY(_currentIndicator));
        }

        if (_automation->selectedLines()->size() > 0 && _mousePressAndHold) {

            double final_xpos = 0 - _diffPressed.width();
            double final_ypos = _currentMouseY - _diffPressed.height();

            _automation->createSnapLine(true);

            _automation->findSnap(_currentMouseX - _diffPressed.width(), _currentMouseY - _diffPressed.height(), final_xpos, final_ypos);

            QPointF newPoint = QPointF(_automation->pixel2Time(final_xpos), final_ypos);
            double dt = newPoint.x() - _automation->selectedLines()->head()->time();

            if (_automation->selectedLines()->head()->isNegInf()) {
                dt = newPoint.x() - _automation->selectedLines()->head()->next()->time();
            }

            double dy = newPoint.y() - _automation->getYPixel(_automation->selectedLines()->head()->next()->y());

            _automation->edit_line(_automation->selectedLines(), QPointF(dt, dy));

            _currentValue = normalized2Label(_automation->selectedLines()->head()->next()->y());
            setAutomatedNormalized(automation()->getY(_currentIndicator));
        }

        Q_EMIT sigUpdate();
    }
}

void AutomationItem::sltLinesChanged()
{
    if (_automation->isAutomated()) {
        qDebug() << "sltLinesChanged" << _currentIndicator;
        setAutomatedNormalized(automation()->getY(_currentIndicator));
    }
}
