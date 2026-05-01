//#include "envelopeitem.h"
//#include <QCursor>
//#include <core/sampleeditor.h>

//EnvelopeItem::EnvelopeItem(QRect rect, SampleEditor* sampleEditor)
//    : AutomationItem(rect, sampleEditor)
//{

//    // _automation = _beatBar->getAutomation_wave_editor();
//}

//void EnvelopeItem::sltTimerCheckPosition()
//{
//    _autoScroll = false;
//    int T = _rect.top();
//    float _sutoScrollSpeedRight = qAbs(rect().width() - _lastMouseMovingPoint.x()) / 20.0;
//    float _sutoScrollSpeedLeft = qAbs(_lastMouseMovingPoint.x()) / 20.0;

//    if (_lastMouseMovingPoint.x() > rect().width()) {

//        _autoScroll = true;

//        float startTime = _sampleEditor->sampleClip()->startTime();
//        float endTime = _sampleEditor->sampleClip()->endTime();
//        float rate = _sutoScrollSpeedRight * (_sampleEditor->sampleClip()->currentClipDuration() / _sampleEditor->lastWidth());

//        //_beatBar->setTimeDuration(rate + timeDuration);

//        _sampleEditor->sampleClip()->setStartTime(startTime + rate);
//        _sampleEditor->sampleClip()->setEndTime(endTime + rate);

//        Q_EMIT sigUpdate();
//    }

//    if (_lastMouseMovingPoint.x() < 0) {
//        _autoScroll = true;

//        float startTime = _sampleEditor->sampleClip()->startTime();
//        float endTime = _sampleEditor->sampleClip()->endTime();
//        float rate = _sutoScrollSpeedLeft * (_sampleEditor->sampleClip()->currentClipDuration() / _sampleEditor->lastWidth());

//        _sampleEditor->sampleClip()->setStartTime(startTime - rate);
//        _sampleEditor->sampleClip()->setEndTime(endTime - rate);

//        Q_EMIT sigUpdate();
//    }
//}

//bool EnvelopeItem::undo()
//{
//    auto action = _sampleEditor->getUndoAction();

//    if (action._action == SampleEditor::ActionName_EnvelopeAction) {
//        _sampleEditor->popUndoAction();
//        if (_automationUndoLines.size() > 0) {

//            qDebug() << "EnvelopeItem::undo()";

//            _sampleEditor->addRedoAction(action._action);
//            _automationRedoLines.push(UndoItem { _sampleEditor->automation()->cloneLines(),
//                _sampleEditor->automation()->cloneSelectedLines(),
//                _sampleEditor->automation()->getSelectedStartIndex(),
//                _sampleEditor->automation()->getSelectedEndIndex() });
//            auto item = _automationUndoLines.pop();
//            _sampleEditor->automation()->setLines(item.lines);
//            _sampleEditor->automation()->setSelectedLines(item.selected_lines, item.startIndex, item.endIndex);

//            _sampleEditor->automation()->setStartPoint(item._startPoint);
//            _sampleEditor->automation()->setEndPoint(item._endPoint);

//            _sampleEditor->automation()->setFirst_point(item._first_point);
//            _sampleEditor->automation()->setLast_point(item._last_point);

//            _sampleEditor->automation()->setFirst_list(item._first_list);
//            _sampleEditor->automation()->setLast_list(item._last_list);

//            if (item.selected_lines.size() == 0) {
//                _sampleEditor->automation()->clearSelectedLines();
//                _sampleEditor->automation()->createSelectedPoints(_sampleEditor->startMarker(), _sampleEditor->endMarker());
//            } else {
//                _sampleEditor->automation()->setInit_selected_line_move(false);
//            }
//        }
//    }

//    return true;
//}

//bool EnvelopeItem::redo()
//{

//    auto action = _sampleEditor->getRedoAction();
//    if (action._action == SampleEditor::ActionName_EnvelopeAction) {
//        _sampleEditor->popRedoAction();
//        if (_automationRedoLines.size() > 0) {

//            _sampleEditor->addUndoAction(action._action);
//            _automationUndoLines.push(UndoItem { _sampleEditor->automation()->cloneLines(),
//                _sampleEditor->automation()->cloneSelectedLines(),
//                _sampleEditor->automation()->getSelectedStartIndex(),
//                _sampleEditor->automation()->getSelectedEndIndex() });

//            auto item = _automationUndoLines.pop();
//            _sampleEditor->automation()->setLines(item.lines);
//            _sampleEditor->automation()->setSelectedLines(item.selected_lines, item.startIndex, item.endIndex);

//            _sampleEditor->automation()->setStartPoint(item._startPoint);
//            _sampleEditor->automation()->setEndPoint(item._endPoint);

//            _sampleEditor->automation()->setFirst_point(item._first_point);
//            _sampleEditor->automation()->setLast_point(item._last_point);

//            _sampleEditor->automation()->setFirst_list(item._first_list);
//            _sampleEditor->automation()->setLast_list(item._last_list);

//            if (item.selected_lines.size() == 0) {
//                _sampleEditor->automation()->clearSelectedLines();
//                _sampleEditor->automation()->createSelectedPoints(_sampleEditor->startMarker(), _sampleEditor->endMarker());
//            } else {
//                _sampleEditor->automation()->setInit_selected_line_move(false);
//            }
//        }
//    }

//    return true;
//}
