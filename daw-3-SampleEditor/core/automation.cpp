#include "automation.h"

void Automation::encofrMonotone(std::vector<std::pair<CPoint*, CPoint*>>& lines)
{

    if (m_selected_point_index && m_selected_point_index->isValid()) {

        std::vector<std::pair<CPoint*, CPoint*>> forward = _cLines.enforceMonotoneFromBaseForward(m_selected_point_index);
        std::vector<std::pair<CPoint*, CPoint*>> backward = _cLines.enforceMonotoneFromBaseBackward(m_selected_point_index);

        lines.reserve(forward.size() + backward.size());

        lines.insert(lines.end(), backward.begin(), backward.end());
        lines.insert(lines.end(), forward.begin(), forward.end());

        if (lines.size() >= 2) {
            lines[0].first->setY(lines[0].second->y());
            lines[lines.size() - 1].second->setY(lines[lines.size() - 1].first->y());
        }

        _tLines = CPointList::buildListFromSegments(lines);
    } else if (!m_selected_line->empty()) {

        std::vector<CLine> first = find_line_index(&_cLines, m_selected_line->head()->time());
        std::vector<CLine> last = find_line_index(&_cLines, m_selected_line->tail()->time());

        if (first.size() > 0 && last.size() > 0) {
            lines = _cLines.buildSegmentsWithInsertedList(first[first.size() - 1].p1(), last[0].p2(), m_selected_line);
        }

        if (lines.size() >= 2) {
            lines[0].first->setY(lines[0].second->y());
            lines[lines.size() - 1].second->setY(lines[lines.size() - 1].first->y());
        }

        _tLines = CPointList::buildListFromSegments(lines);
    } else {

        if (_selectedLines->size() > 0) {

            std::vector<CLine> first = find_line_index(&_cLines, _selectedLines->head()->time());
            std::vector<CLine> last = find_line_index(&_cLines, _selectedLines->tail()->time());

            std::vector<std::pair<CPoint*, CPoint*>> backward;
            std::vector<std::pair<CPoint*, CPoint*>> forward;

            lines.reserve(forward.size() + backward.size() + _selectedLines->size());
            auto pairs = _selectedLines->toPairs();

            bool exclude_first = false;

            if (first.size() >= 2) {
                backward = _cLines.enforceMonotoneFromBaseBackward(first[first.size() - 1].p1());

                _selectedLines->head()->setTime(first[first.size() - 1].p1()->time());
                _selectedLines->head()->setY(first[first.size() - 1].p1()->y());

                for (const auto& pr : backward) {
                    lines.push_back(pr);
                }

            } else if (first.size() > 0) {
                backward = _cLines.enforceMonotoneFromBaseBackward(first[0].p1());

                for (const auto& pr : backward) {
                    lines.push_back(pr);
                }

                if (first[0].p1()->moveTag() == CPoint::MoveTagState_First) {

                    exclude_first = true;
                    lines.push_back(std::make_pair(first[0].p1(), _selectedLines->head()->next()));

                } else {

                    _selectedLines->head()->setTime(_selectedLines->head()->time());
                    double r1 = MathHelper1::yCalculatorFirst(first[0].p1()->pos(), first[0].p2()->pos(), _selectedLines->head()->time());
                    float p = 1.0, q = 1.0, r = 1.0, s = 1.0;
                    if (first[0].p1()->curve_n() != 1.0) {

                        float cx = time2Pixel(_selectedLines->head()->time());
                        float cy;
                        MathHelper1::findNewRoot1(cPoint2QPoint(first[0].p2()), cPoint2QPoint(first[0].p1()), cx, first[0].p1()->curve_n(), first[0].p1()->curve_m(), p, q, r, s, cy);
                        _selectedLines->head()->setY(pixel2Y(cy));
                        _selectedLines->head()->setCurve_n(p);
                        _selectedLines->head()->setCurve_m(q);
                    } else {
                        _selectedLines->head()->setY(r1);
                    }

                    lines.push_back(std::make_pair(first[0].p1(), _selectedLines->head()));
                }
            }

            for (size_t i = 0; i < pairs.size(); ++i) {

                if (i == 0 && exclude_first) {
                    continue;
                }
                if (i == pairs.size() - 1) // skip last one
                    continue; // (add it later)

                lines.push_back(pairs[i]);
            }

            if (last.size() >= 2) {

                forward = _cLines.enforceMonotoneFromBaseForward(last[0].p2());

                _selectedLines->tail()->setTime(last[0].p2()->time());
                _selectedLines->tail()->setY(last[0].p2()->y());

                if (pairs.size() > 0) {
                    lines.push_back(pairs[pairs.size() - 1]);
                }

            } else if (last.size() > 0) {
                forward = _cLines.enforceMonotoneFromBaseForward(last[0].p2());

                if (last[0].p2()->moveTag() == CPoint::MoveTagState_Last) {

                    lines.push_back(std::make_pair(_selectedLines->tail()->prev(), last[0].p2()));

                } else {

                    forward = _cLines.enforceMonotoneFromBaseForward(last[0].p2());
                    double r2 = MathHelper1::yCalculatorLast(last[0].p1()->pos(), last[0].p2()->pos(), _selectedLines->tail()->time());

                    float p = 1.0, q = 1.0, r = 1.0, s = 1.0;
                    if (last[0].p1()->curve_n() != 1.0) {

                        float cx = time2Pixel(_selectedLines->tail()->time());
                        float cy;
                        MathHelper1::findNewRoot1(cPoint2QPoint(last[0].p2()), cPoint2QPoint(last[0].p1()), cx, last[0].p1()->curve_n(), last[0].p1()->curve_m(), p, q, r, s, cy);

                        _selectedLines->tail()->setY(pixel2Y(cy));
                        _selectedLines->tail()->setCurve_n(r);
                        _selectedLines->tail()->setCurve_m(s);
                    } else {
                        _selectedLines->tail()->setY(r2);
                    }

                    if (pairs.size() > 0) {

                        lines.push_back(pairs[pairs.size() - 1]);
                    }

                    lines.push_back(std::make_pair(_selectedLines->tail(), last[0].p2()));
                }
            }

            for (const auto& pr : forward) {
                lines.push_back(pr);
            }

            _tLines = CPointList::buildListFromSegments(lines);

        } else {

            lines = _cLines.toPairs();
        }
    }
}

std::vector<CLine> Automation::toQLinesScaleY(double xMin, double xMax)
{

    std::vector<std::pair<CPoint*, CPoint*>> lines;

    if (_cLines.size() > 0) {
        encofrMonotone(lines);
    }

    std::vector<CLine> qlines;
    qlines.reserve(lines.size());

    auto toPointF = [&](const CPoint& p) -> QPointF {
        double x = 0;
        if (std::isinf(p.time())) {
            x = std::signbit(p.time()) ? xMin : xMax;
        } else {
            x = time2Pixel(p.time());
        }

        double y = getYPixel(p.y());

        if (y > _windowRect.bottom()) {
            y = _windowRect.bottom();
        }
        if (y < _windowRect.top()) {
            y = _windowRect.top();
        }

        return QPointF(x, y);
    };

    for (const auto& pr : lines) {

        CPoint* p0 = pr.first;
        CPoint* p1 = pr.second;
        auto c = CLine(p0, p1);

        c.setFp1(toPointF(*p0));
        c.setFp2(toPointF(*p1));

        c.setCurveN(p0->curve_n());
        c.setCurveM(p0->curve_m());

        c.setStartX(time2Pixel(p0->effectiveTime()));
        c.setEndX(time2Pixel(p1->effectiveTime()));

        if (c.p1()->enabled() && c.p2()->enabled()) {
            c.setEnabled(true);
        } else {
            c.setEnabled(false);
        }
        if (c.p1()->hovred() && c.p2()->hovred())
            c.setHovred(true);
        else
            c.setHovred(false);

        qlines.emplace_back(c);
    }

    return qlines;
}

std::vector<CLine> Automation::toSelectedLinesScale(double xMin, double xMax)
{

    std::vector<std::pair<CPoint*, CPoint*>> lines;

    if (_selectedLinesHovered && _selectedLines->size() == 0)
        lines = _hoveredLines.toPairs();

    std::vector<CLine> qlines;
    qlines.reserve(lines.size());

    auto toPointF = [&](const CPoint& p) -> QPointF {
        double x = 0;
        if (std::isinf(p.time())) {
            x = std::signbit(p.time()) ? xMin : xMax;
        } else {
            x = time2Pixel(p.time());
        }

        double y = getYPixel(p.y());

        return QPointF(x, y);
    };

    for (const auto& pr : lines) {

        CPoint* p0 = pr.first;
        CPoint* p1 = pr.second;
        auto c = CLine(p0, p1);

        c.setFp1(toPointF(*p0));
        c.setFp2(toPointF(*p1));

        c.setCurveN(p0->curve_n());
        c.setCurveM(p0->curve_m());

        c.setStartX(time2Pixel(p0->effectiveTime()));
        c.setEndX(time2Pixel(p1->effectiveTime()));

        if (c.p1()->enabled() && c.p2()->enabled()) {
            c.setEnabled(true);
        } else {
            c.setEnabled(false);
        }

        if (c.p1()->hovred() && c.p2()->hovred())
            c.setHovred(true);
        else
            c.setHovred(false);

        qlines.emplace_back(c);
    }

    return qlines;
}

void Automation::setSelectedLine(CPoint* start, CPoint* end)
{
    _cLines.extractRange(m_selected_line, start, end);
}

void Automation::clearHoveredLines()
{
    _selectedLinesHovered = false;
    _hoveredLines.clear();
}

void Automation::createHoveredLines(double start_time, double end_time)
{

    clearHoveredLines();
    if (qAbs(start_time - end_time) < 1e-9) {
        return;
    }

    std::vector<CLine> first = find_line_index(&_cLines, start_time);
    std::vector<CLine> last = find_line_index(&_cLines, end_time);

    if (first.size() == 1 && last.size() == 1) {

        if (first[0].p1() == last[0].p1()) {
            double r1 = MathHelper1::yCalculatorFirst(first[0].p1()->pos(), first[0].p2()->pos(), start_time);
            double r2 = MathHelper1::yCalculatorLast(last[0].p1()->pos(), last[0].p2()->pos(), end_time);

            if (first[0].p1()->curve_n() != 1.0) {
                auto p1 = new CPoint(first[0].p1()->time(), first[0].p1()->y());

                p1->setEffectiveTime(start_time);
                p1->setCurve_m(first[0].p1()->curve_m());
                p1->setCurve_n(first[0].p1()->curve_n());
                auto p2 = new CPoint(first[0].p2()->time(), first[0].p2()->y());
                p2->setEffectiveTime(end_time);
                p2->setCurve_m(first[0].p2()->curve_m());
                p2->setCurve_n(first[0].p2()->curve_n());
                _hoveredLines.push_back(p1);
                _hoveredLines.push_back(p2);
            } else {
                _hoveredLines.push_back(new CPoint(start_time, r1));
                _hoveredLines.push_back(new CPoint(end_time, r2));
            }

        } else {

            double r1 = MathHelper1::yCalculatorFirst(first[0].p1()->pos(), first[0].p2()->pos(), start_time);
            double r2 = MathHelper1::yCalculatorLast(last[0].p1()->pos(), last[0].p2()->pos(), end_time);
            _cLines.cloneRange(&_hoveredLines, first[0].p2(), last[0].p1());

            if (first[0].p1()->curve_n() != 1.0) {

                auto p1 = new CPoint(first[0].p1()->time(), first[0].p1()->y());

                p1->setEffectiveTime(start_time);
                p1->setCurve_m(first[0].p1()->curve_m());
                p1->setCurve_n(first[0].p1()->curve_n());
                _hoveredLines.push_front(p1);
            } else {
                _hoveredLines.push_front(new CPoint(start_time, r1));
            }

            if (last[0].p1()->curve_n() != 1.0) {
                auto p2 = new CPoint(last[0].p2()->time(), last[0].p2()->y());
                p2->setEffectiveTime(end_time);
                p2->setCurve_m(last[0].p1()->curve_m());
                p2->setCurve_n(last[0].p1()->curve_n());

                _hoveredLines.push_back(p2);

            } else {
                _hoveredLines.push_back(new CPoint(end_time, r2));
            }
        }
    }

    else if (first.size() >= 2 && last.size() == 1) {

        double r2 = MathHelper1::yCalculatorLast(last[0].p1()->pos(), last[0].p2()->pos(), end_time);

        _cLines.cloneRange(&_hoveredLines, first[first.size() - 1].p1(), last[0].p1());

        if (last[0].p1()->curve_n() != 1.0) {
            auto p2 = new CPoint(last[0].p2()->time(), last[0].p2()->y());
            p2->setEffectiveTime(end_time);
            p2->setCurve_m(last[0].p1()->curve_m());
            p2->setCurve_n(last[0].p1()->curve_n());

            _hoveredLines.push_back(p2);

        } else {
            _hoveredLines.push_back(new CPoint(end_time, r2));
        }
    }

    else if (first.size() == 1 && last.size() >= 2) {
        double r1 = MathHelper1::yCalculatorFirst(first[0].p1()->pos(), first[0].p2()->pos(), start_time);

        _cLines.cloneRange(&_hoveredLines, first[0].p2(), last[0].p2());

        if (first[0].p1()->curve_n() != 1.0) {

            auto p1 = new CPoint(first[0].p1()->time(), first[0].p1()->y());

            p1->setEffectiveTime(start_time);
            p1->setCurve_m(first[0].p1()->curve_m());
            p1->setCurve_n(first[0].p1()->curve_n());
            _hoveredLines.push_front(p1);
        } else {
            _hoveredLines.push_front(new CPoint(start_time, r1));
        }

    }

    else if (first.size() >= 2 && last.size() >= 2) {

        _cLines.cloneRange(&_hoveredLines, first[first.size() - 1].p1(), last[0].p2());
    }
}

void Automation::deleteLines(double start_time, double end_time)
{
    auto _startMarker = start_time;
    auto _endMarker = end_time;

    if (qAbs(_startMarker - _endMarker) <= 0.0)
        return;

    std::vector<CLine> first = find_line_index(&_cLines, start_time);
    std::vector<CLine> last = find_line_index(&_cLines, end_time);

    if (_cLines.size() <= 2) {
        return;
    }

    if (first.size() == 1 && last.size() == 1) {

        CPointList list;
        _selectedLines->clear();

        _cLines.extractRange(&list, first[0].p2(), last[0].p1());
        list.clear();

        _cLines.tail()->setY(_cLines.tail()->prev()->y());
        _cLines.head()->setY(_cLines.head()->next()->y());

        createHoveredLines(start_time, end_time);
    } else if (first.size() >= 2 && last.size() == 1) {

        CPointList list;
        _selectedLines->clear();

        _cLines.extractRange(&list, first[0].p2(), last[0].p1());
        list.clear();

        _cLines.tail()->setY(_cLines.tail()->prev()->y());
        _cLines.head()->setY(_cLines.head()->next()->y());

        createHoveredLines(start_time, end_time);
    }

    else if (first.size() == 1 && last.size() >= 2) {

        CPointList list;
        _selectedLines->clear();

        _cLines.extractRange(&list, first[0].p2(), last[last.size() - 1].p1());
        list.clear();

        _cLines.tail()->setY(_cLines.tail()->prev()->y());
        _cLines.head()->setY(_cLines.head()->next()->y());

        createHoveredLines(start_time, end_time);
    }

    else if (first.size() >= 2 && last.size() >= 2) {

        CPointList list;
        _selectedLines->clear();

        _cLines.extractRange(&list, first[0].p2(), last[last.size() - 1].p1());
        list.clear();

        _cLines.tail()->setY(_cLines.tail()->prev()->y());
        _cLines.head()->setY(_cLines.head()->next()->y());

        createHoveredLines(start_time, end_time);
    }

    if (_cLines.size() == 2) {
        _cLines.head()->setY(_normalized);
        _cLines.tail()->setY(_normalized);
    }

    Q_EMIT sigLinesChanged();
}

void Automation::insertLines(double start_time, double end_time, double indexSub)
{

    if (_copiedLines.size() == 0)
        return;

    std::vector<std::pair<CPoint*, CPoint*>> lines;

    edit_line(&_copiedLines, QPointF(start_time - _copiedLines.head()->time(), 0));

    std::vector<CLine> first = find_line_index(&_cLines, _copiedLines.head()->time(), _copiedLines.head()->y());
    std::vector<CLine> last = find_line_index(&_cLines, _copiedLines.tail()->time(), _copiedLines.tail()->y());

    std::vector<std::pair<CPoint*, CPoint*>> backward;
    std::vector<std::pair<CPoint*, CPoint*>> forward;

    lines.reserve(forward.size() + backward.size() + _copiedLines.size());
    auto pairs = _copiedLines.toPairs();

    if (first.size() >= 2) {
        backward = _cLines.enforceMonotoneFromBaseBackward(first[first.size() - 1].p1());
        _copiedLines.head()->setTime(first[first.size() - 1].p1()->time());
        _copiedLines.head()->setY(first[first.size() - 1].p1()->y());

        for (const auto& pr : backward) {
            lines.push_back(pr);
        }

    } else if (first.size() > 0) {
        backward = _cLines.enforceMonotoneFromBaseBackward(first[0].p1());

        for (const auto& pr : backward) {
            lines.push_back(pr);
        }

        double r1 = MathHelper1::yCalculatorFirst(first[0].p1()->pos(), first[0].p2()->pos(), _copiedLines.head()->time());

        float p = 1.0, q = 1.0, r = 1.0, s = 1.0;
        if (first[0].p1()->curve_n() != 1.0) {

            float cx = time2Pixel(_copiedLines.head()->time());
            float cy;
            MathHelper1::findNewRoot1(cPoint2QPoint(first[0].p2()), cPoint2QPoint(first[0].p1()), cx, first[0].p1()->curve_n(), first[0].p1()->curve_m(), p, q, r, s, cy);
            _copiedLines.head()->setY(pixel2Y(cy));
            _copiedLines.head()->setCurve_n(p);
            _copiedLines.head()->setCurve_m(q);
        } else {
            _copiedLines.head()->setY(r1);
        }

        lines.push_back(std::make_pair(first[0].p1(), _copiedLines.head()));
    }

    for (const auto& pr : pairs) {
        lines.push_back(pr);
    }

    if (last.size() >= 2) {
        forward = _cLines.enforceMonotoneFromBaseForward(last[0].p2());

        _copiedLines.tail()->setTime(last[0].p2()->time());
        _copiedLines.tail()->setY(last[0].p2()->y());

    } else if (last.size() > 0) {
        forward = _cLines.enforceMonotoneFromBaseForward(last[0].p2());
        double r2 = MathHelper1::yCalculatorLast(last[0].p1()->pos(), last[0].p2()->pos(), _copiedLines.tail()->time());

        float p = 1.0, q = 1.0, r = 1.0, s = 1.0;
        if (last[0].p1()->curve_n() != 1.0) {

            float cx = time2Pixel(_copiedLines.tail()->time());
            float cy;
            MathHelper1::findNewRoot1(cPoint2QPoint(last[0].p2()), cPoint2QPoint(last[0].p1()), cx, last[0].p1()->curve_n(), last[0].p1()->curve_m(), p, q, r, s, cy);

            _copiedLines.tail()->setY(pixel2Y(cy));
            _copiedLines.tail()->setCurve_n(r);
            _copiedLines.tail()->setCurve_m(s);
        } else {
            _copiedLines.tail()->setY(r2);
        }

        lines.push_back(std::make_pair(_copiedLines.tail(), last[0].p2()));
    }

    for (const auto& pr : forward) {
        lines.push_back(pr);
    }

    _cLines = CPointList::buildListFromSegments(lines);

    Q_EMIT sigLinesChanged();
}

bool Automation::createCopyLines(double start_time, double end_time)
{

    _startMarker = start_time;
    _endMarker = end_time;

    if (qAbs(_startMarker - _endMarker) <= 0.0)
        return false;

    std::vector<CLine> first = find_line_index(&_cLines, start_time);
    std::vector<CLine> last = find_line_index(&_cLines, end_time);

    //    if (_cLines.size() <= 2) {
    //        return false;
    //    }

    if (first.size() == 1 && last.size() == 1) {
        double r1 = MathHelper1::yCalculatorFirst(first[0].p1()->pos(), first[0].p2()->pos(), start_time);
        double r2 = MathHelper1::yCalculatorLast(last[0].p1()->pos(), last[0].p2()->pos(), end_time);

        if (first[0].p1() == last[0].p1() && first[0].p2() == last[0].p2()) {

        } else {
            _cLines.cloneRange(&_copiedLines, first[0].p2(), last[0].p1());
        }

        _copiedLines.push_front(new CPoint(start_time, r1));
        _copiedLines.push_back(new CPoint(end_time, r2));

        _copiedLines.push_front(new CPoint(start_time, r1));
        _copiedLines.push_back(new CPoint(end_time, r2));
    }
    if (first.size() >= 2 && last.size() == 1) {

        double r2 = MathHelper1::yCalculatorLast(last[0].p1()->pos(), last[0].p2()->pos(), end_time);

        _cLines.cloneRange(&_copiedLines, first[first.size() - 1].p1(), last[0].p1());

        _copiedLines.push_back(new CPoint(end_time, r2));

        _copiedLines.push_front(new CPoint(start_time, first[first.size() - 1].p1()->y()));
        _copiedLines.push_back(new CPoint(end_time, r2));
    }

    else if (first.size() == 1 && last.size() >= 2) {
        double r1 = MathHelper1::yCalculatorFirst(first[0].p1()->pos(), first[0].p2()->pos(), start_time);

        _cLines.cloneRange(&_copiedLines, first[0].p2(), last[0].p2());

        _copiedLines.push_front(new CPoint(start_time, r1));

        _copiedLines.push_front(new CPoint(start_time, r1));
        _copiedLines.push_back(new CPoint(end_time, last[0].p2()->y()));
    }

    else if (first.size() >= 2 && last.size() >= 2) {

        _cLines.cloneRange(&_copiedLines, first[first.size() - 1].p1(), last[0].p2());

        _copiedLines.push_front(new CPoint(start_time, first[first.size() - 1].p1()->y()));
        _copiedLines.push_back(new CPoint(end_time, last[0].p2()->y()));
    }

    return true;
}

void Automation::createSelectedLines()
{

    if (_hoveredLines.size() == 0)
        return;

    _selectedLines->clear();

    double start_time = _hoveredLines.head()->effectiveTime();
    double end_time = _hoveredLines.tail()->effectiveTime();

    clearHoveredLines();
    std::vector<CLine> first = find_line_index(&_cLines, start_time);
    std::vector<CLine> last = find_line_index(&_cLines, end_time);

    if (first.size() == 1 && last.size() == 1) {

        double y1 = MathHelper1::yCalculatorFirst(first[0].p1()->pos(), first[0].p2()->pos(), start_time);
        double y2 = MathHelper1::yCalculatorLast(last[0].p1()->pos(), last[0].p2()->pos(), end_time);

        float p = 1.0, q = 1.0, r = 1.0, s = 1.0;
        float p1 = 1.0, q1 = 1.0, r1 = 1.0, s1 = 1.0;
        float p2 = 1.0, q2 = 1.0, r2 = 1.0, s2 = 1.0;
        QPointF c = cPoint2QPoint(last[0].p1());

        if (first[0].p1()->curve_n() != 1.0) {
            float cx = time2Pixel(start_time);
            float cy;
            MathHelper1::findNewRoot1(cPoint2QPoint(first[0].p2()), cPoint2QPoint(first[0].p1()), cx, first[0].p1()->curve_n(), first[0].p1()->curve_m(), p, q, r, s, cy);
            c = QPointF(cx, cy);

            y1 = pixel2Y(cy);
            first[0].p1()->setCurve_n(p);
            first[0].p1()->setCurve_m(q);
        }

        if (first[0].p1() == last[0].p1() && first[0].p2() == last[0].p2()) {

            r2 = r;
            s2 = s;

        } else {
            r2 = last[0].p1()->curve_n();
            s2 = last[0].p1()->curve_m();
            c = cPoint2QPoint(last[0].p1());
        }

        if (last[0].p1()->curve_n() != 1.0) {
            float cx = time2Pixel(end_time);
            float cy;
            MathHelper1::findNewRoot1(cPoint2QPoint(last[0].p2()), c, cx, r2, s2, p1, q1, r1, s1, cy);
            QPointF c = QPointF(cx, cy);

            y2 = pixel2Y(cy);
            last[0].p1()->setCurve_n(p1);
            last[0].p1()->setCurve_m(q1);
        }

        if (first[0].p1() == last[0].p1() && first[0].p2() == last[0].p2()) {

        } else {
            _cLines.extractRange(_selectedLines, first[0].p2(), last[0].p1());
        }

        auto ps1 = new CPoint(start_time, y1);
        auto ps2 = new CPoint(start_time, y1);

        auto pe1 = new CPoint(end_time, y2);
        auto pe2 = new CPoint(end_time, y2);

        auto pm1 = new CPoint(start_time, y1);
        auto pm2 = new CPoint(end_time, y2);

        ps1->setCurve_n(r);
        ps1->setCurve_m(s);

        pe2->setCurve_n(r1);
        pe2->setCurve_m(s1);

        pm1->setMoveTag(CPoint::MoveTagState_First);

        pm2->setCurve_n(r1);
        pm2->setCurve_m(s1);
        pm2->setMoveTag(CPoint::MoveTagState_Last);

        _selectedLines->push_front(ps1);
        _selectedLines->push_back(pe1);

        _selectedLines->push_front(ps2);
        _selectedLines->push_back(pe2);

        _cLines.insertAfter(first[0].p1(), pm1);
        _cLines.insertBefore(last[0].p2(), pm2);

    } else if (first.size() >= 2 && last.size() == 1) {

        double r1 = first[first.size() - 1].p1()->y();

        auto prev = first[first.size() - 1].p1()->prev();
        float p = 1.0, q = 1.0, r = 1.0, s = 1.0;

        double r2 = MathHelper1::yCalculatorLast(last[0].p1()->pos(), last[0].p2()->pos(), end_time);
        if (last[0].p1()->curve_n() != 1.0) {
            float cx = time2Pixel(end_time);
            float cy;
            MathHelper1::findNewRoot1(cPoint2QPoint(last[0].p2()), cPoint2QPoint(last[0].p1()), cx, last[0].p1()->curve_n(), last[0].p1()->curve_m(), p, q, r, s, cy);
            QPointF c = QPointF(cx, cy);

            r2 = pixel2Y(cy);
            last[0].p1()->setCurve_n(p);
            last[0].p1()->setCurve_m(q);
        }

        _cLines.extractRange(_selectedLines, first[first.size() - 1].p1(), last[0].p1());

        _selectedLines->push_front(new CPoint(start_time, r1));

        auto pe1 = new CPoint(end_time, r2);
        auto pe2 = new CPoint(end_time, r2);

        _selectedLines->push_back(pe1);
        _selectedLines->push_back(pe2);

        pe2->setCurve_n(r);
        pe2->setCurve_m(s);

        auto pm2 = new CPoint(end_time, r2);
        pm2->setCurve_n(r);
        pm2->setCurve_m(s);
        pm2->setMoveTag(CPoint::MoveTagState_Last);

        _cLines.insertAfter(prev, new CPoint(start_time, r1, QPointF(), CPoint::MoveTagState_First));
        _cLines.insertBefore(last[0].p2(), pm2);

    }

    else if (first.size() == 1 && last.size() >= 2) {

        double r1 = MathHelper1::yCalculatorFirst(first[0].p1()->pos(), first[0].p2()->pos(), start_time);
        double r2 = last[0].p2()->y();

        float p = 1.0, q = 1.0, r = 1.0, s = 1.0;

        if (first[0].p1()->curve_n() != 1.0) {
            float cx = time2Pixel(start_time);
            float cy;
            MathHelper1::findNewRoot1(cPoint2QPoint(first[0].p2()), cPoint2QPoint(first[0].p1()), cx, first[0].p1()->curve_n(), first[0].p1()->curve_m(), p, q, r, s, cy);
            QPointF c = QPointF(cx, cy);

            r1 = pixel2Y(cy);
            first[0].p1()->setCurve_n(p);
            first[0].p1()->setCurve_m(q);
        }

        _cLines.extractRange(_selectedLines, first[0].p2(), last[0].p2());

        auto pe1 = new CPoint(start_time, r1);
        auto pe2 = new CPoint(start_time, r1);
        pe1->setCurve_n(r);
        pe1->setCurve_m(s);

        _selectedLines->push_front(pe1);
        _selectedLines->push_front(pe2);

        _selectedLines->push_back(new CPoint(end_time, r2));

        auto pm2 = new CPoint(start_time, r1);
        pm2->setMoveTag(CPoint::MoveTagState_First);

        _cLines.insertAfter(first[0].p1(), pm2);
        _cLines.insertBefore(last[1].p2(), new CPoint(end_time, r2, QPointF(), CPoint::MoveTagState_Last));
    }

    else if (first.size() >= 2 && last.size() >= 2) {

        double r1 = first[first.size() - 1].p1()->y();
        double r2 = last[0].p2()->y();

        auto prev = first[first.size() - 1].p1()->prev();

        _cLines.extractRange(_selectedLines, first[first.size() - 1].p1(), last[0].p2());

        _selectedLines->push_front(new CPoint(start_time, r1));
        _selectedLines->push_back(new CPoint(end_time, r2));

        _cLines.insertAfter(prev, new CPoint(start_time, r1, QPointF(), CPoint::MoveTagState_First));
        _cLines.insertBefore(last[1].p2(), new CPoint(end_time, r2, QPointF(), CPoint::MoveTagState_Last));
    }
}

Automation::HitResult Automation::hitTestLines(double mouseX, double mouseY, double onLineThreshold, double nearLineThreshold, double pointRadius)
{

    QPointF mouse(mouseX, mouseY);
    HitResult best;

    auto lines = _cLines.toPairs();
    std::vector<CLine> qlines;
    qlines.reserve(lines.size());

    auto toPointF = [&](const CPoint& p) -> QPointF {
        double x = 0;
        if (std::isinf(p.time())) {
            x = std::signbit(p.time()) ? 0 : _areaInfo->lastWidth();
        } else {
            x = time2Pixel(p.time());
        }

        double y = getYPixel(p.y());

        return QPointF(x, y);
    };

    for (const auto& pr : lines) {

        CPoint* p0 = pr.first;
        CPoint* p1 = pr.second;
        auto c = CLine(p0, p1);
        c.setFp1(toPointF(*p0));
        c.setFp2(toPointF(*p1));
        c.setCurveN(p0->curve_n());
        c.setCurveM(p0->curve_m());

        qlines.emplace_back(c);
    }

    for (size_t i = 0; i < qlines.size(); ++i) {
        const QLineF& ln = QLineF(qlines[i].fp1(), qlines[i].fp2());
        const QPointF& a = ln.p1();
        const QPointF& b = ln.p2();

        // --- 1. Endpoint check ---
        double d1 = QLineF(mouse, a).length();
        double d2 = QLineF(mouse, b).length();
        if (d1 <= pointRadius) {
            qlines[i].p1()->setHovred(true);
            return { qlines[i], HitState::OnPoint, 0, a };
        }
        if (d2 <= pointRadius) {
            qlines[i].p2()->setHovred(true);
            return { qlines[i], HitState::OnPoint, 1, b };
        }

        float curve_n = 1, curve_m = 1;
        curve_n = qlines[i].curveN();
        curve_m = qlines[i].curveM();

        auto snapThreshold = ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor());

        if (curve_n != 1.0 || curve_m != 1.0) {

            auto onThreshold = ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor());
            auto nearThreshold = ScreenInterface::scaleSize2(40, _areaInfo->zoomFactor());

            bool nearS = MathHelper1::isInSuperEllipseArea(b, a, curve_n, curve_m, mouseX, mouseY, nearThreshold);

            if (nearS) {

                bool lineS = MathHelper1::isInSuperEllipseArea(b, a, curve_n, curve_m, mouseX, mouseY, onThreshold);

                if (lineS) {
                    double final_xpos = mouseX, final_ypos = mouseY;

                    createSnapLine(true);

                    // auto innerThreshold = ScreenInterface::scaleSize2(10, _areaInfo->zoomFactor());
                    findSnapWithThreshold(mouseX, mouseY, final_xpos, final_ypos, snapThreshold);

                    QPointF result;
                    result = MathHelper1::mapPointToCurve(qlines[i].fp1(), qlines[i].fp2(), QPointF(final_xpos, final_ypos), curve_n, curve_m);

                    return { qlines[i], HitState::OnLine, -1, QPointF(result.x(), result.y()) };

                } else {
                    return { qlines[i], HitState::NearLine, -1, QPointF(-1, -1) };
                }
            }
        } else {
            QPolygonF _polyInner, _polyOuter;

            MathHelper1::lineAreas(a, b, _polyInner, _polyOuter, onLineThreshold, nearLineThreshold);
            if (qAbs(b.x() - a.x()) > 0) {
                if (_polyInner.containsPoint(QPointF(mouseX, mouseY), Qt::WindingFill)) {

                    double final_xpos = mouseX, final_ypos = mouseY;

                    createSnapLine(true);

                    // auto innerThreshold = ScreenInterface::scaleSize2(10, _areaInfo->zoomFactor());
                    findSnapWithThreshold(mouseX, mouseY, final_xpos, final_ypos, snapThreshold);

                    QPointF mapped = MathHelper1::mapPointToLine(qlines[i].fp1(), qlines[i].fp2(), QPointF(final_xpos, final_ypos));

                    return { qlines[i], HitState::OnLine, -1, mapped };

                } else if (_polyOuter.containsPoint(QPointF(mouseX, mouseY), Qt::WindingFill)) {

                    return { qlines[i], HitState::NearLine, -1, QPointF(-1, -1) };
                }
            } else {
                if (_polyOuter.containsPoint(QPointF(mouseX, mouseY), Qt::WindingFill)) {
                    return { qlines[i], HitState::NearLine, -1, QPointF(-1, -1) };
                }
            }
        }
    }

    return best;
}

Automation::Automation(AreaInfo* areaInfo, int trackIndex, QString type, QString subType, float minY, float maxY, float defaultY, QObject* parent)
    : QObject(parent)
    , _areaInfo(areaInfo)
    , _type(type)
    , _subType(subType)
    , m_minY(minY)
    , m_maxY(maxY)

{
    _defaultNormalized = defaultY;
    _trackIndex = trackIndex;

    _selectedLines = new CPointList();
    m_selected_line = new CPointList();
    _cLines.push_back(CPoint::NegInf(_defaultNormalized));
    _cLines.push_back(CPoint::PosInf(_defaultNormalized));
}

Automation::~Automation()
{
    qDebug() << "~Automation()";
}

std::vector<CLine> Automation::find_line_index(const CPointList* in, double t, double y, double eps)
{
    std::vector<CLine> result;
    std::vector<CLine> final;

    if (in->empty()) {
        return result;
    }

    double global_min_t = in->head()->time();
    double global_max_t = in->tail()->time();

    if (t > global_max_t + eps) {

        CLine line = CLine(in->tail(), nullptr);

        result.push_back(line);

        return result;
    }

    if (t < global_min_t - eps) {

        CLine line = CLine(nullptr, in->head());

        result.push_back(line);

        return result;
    }

    auto points = in->toList();

    for (std::size_t i = 1; i < points.size(); ++i) {
        CPoint* a = points[i - 1];
        CPoint* b = points[i];

        double t0 = a->time();
        double t1 = b->time();

        if (t < t0 - eps) {
            continue;
        }

        if (t > t1 + eps) {
            continue;
        }

        CLine line = CLine(a, b);

        result.push_back(line);
    }
    for (std::size_t i = 0; i < result.size(); ++i) {
        CPoint* a = result[i].p1();
        CPoint* b = result[i].p2();

        // vertical line?
        if (qAbs(a->time() - b->time()) < eps && y > 0) {

            double y1 = a->y();
            double y2 = b->y();

            double ymin = std::min(y1, y2);
            double ymax = std::max(y1, y2);

            // check if point's y is over (on) the segment, including endpoints
            if (y < ymin - eps || y > ymax + eps) {
                continue; // outside this vertical segment
            }
        }

        // if we reach here, either not vertical, or point is over the vertical segment
        final.push_back(result[i]);
    }
    return final;
}

void Automation::edit_point(CPoint* point, double t, double y, double eps)
{

    if (!validY(y))
        return;

    if (_cLines.empty())
        return;

    if (point->isNegInf() || point->isPosInf()) {
    } else {
        if (t < 0) {
            t = 0;
        }
        point->setTime(t);
    }

    point->setY(y);

    if (point->next()) {
        if (point->next()->isPosInf()) {
            point->next()->setY(y);
        }
    }

    if (point->prev()) {
        if (point->prev()->isNegInf()) {
            point->prev()->setY(y);
        }
    }

    // Emit so downstream consumers (e.g. KeyFramesAutomation bridge in
    // TrackItem → syncObjectKeyframes → ObjectPosAutomation::moveTimeById)
    // see the time change. Without this, dragging a kf via OnPoint hit-test
    // mutates _cLines silently and the engine never learns the kf moved.
    Q_EMIT sigLinesChanged();
}

void Automation::edit_line(CPointList* list, QPointF d, double eps)
{
    if (list->empty())
        return;

    //    double dt = newPoint.x() - list->head()->time();
    //    double dy = newPoint.y() - getYPixel(list->head()->y());

    double dt = d.x();
    double dy = d.y();

    for (CPoint* p = list->head(); p; p = p->next()) {

        // shift time

        if (p->isNegInf() || p->isPosInf()) {

        } else {
            double newTime = p->time() + dt;
            if (newTime < 0) {
                newTime = 0;
            }

            if (std::fabs(newTime - p->time()) > eps)
                p->setTime(newTime);
        }

        // shift y in pixel space, then convert back
        const double newY = pixel2Y(getYPixel(p->y()) + dy);

        if (std::fabs(newY - p->y()) > eps)
            p->setY(newY);
    }

    Q_EMIT sigLinesChanged();
}

CPoint* Automation::add_point(double t, double y, double eps)
{

    if (!validY(y))
        return nullptr;

    CPointList* list;
    if (!_tLines.empty()) {

        list = &_tLines;
    } else {

        list = &_cLines;
    }

    if (list->empty())
        return nullptr;

    auto hit = find_line_index(list, t, eps);

    if (hit.empty())
        return nullptr;

    CLine seg = hit[0];

    // If exactly at left endpoint -> nothing to split, but ensure y consistency
    if (nearlyEqual(t, seg.p1()->time(), eps)) {
        return nullptr;
    }

    // If exactly at right endpoint -> snap/merge with the next if any
    if (nearlyEqual(t, seg.p2()->time(), eps)) {
        return nullptr;
    }

    // Ensure the point is strictly inside the segment by time
    if (!(seg.p1()->time() < t && t < seg.p2()->time()))
        return nullptr;

    auto c = new CPoint(t, y);

    float p = 1.0, q = 1.0, r = 1.0, s = 1.0;
    if (seg.p1()->curve_n() != 1.0) {

        float cx = time2Pixel(t);

        float cy;
        MathHelper1::findNewRoot1(cPoint2QPoint(seg.p2()), cPoint2QPoint(seg.p1()), cx, seg.p1()->curve_n(), seg.p1()->curve_m(), p, q, r, s, cy);

        seg.p1()->setCurve_n(p);
        seg.p1()->setCurve_m(q);
        c->setY(pixel2Y(cy));
        c->setCurve_n(r);
        c->setCurve_m(s);

        qDebug() << "cx" << cx << cy << y;
    }

    _cLines.insertBetween(seg.p1(), seg.p2(), c);

    Q_EMIT sigLinesChanged();
    // Keep connectivity tight
    // validate_and_fix(eps);
    return c;
}

void Automation::update(QRect windowRect)
{

    _windowRect = windowRect;
}

QPointF Automation::getFirstSelectedPoint()
{
    if (m_selected_line && m_selected_line->size() > 0) {
        return QPointF(time2Pixel(m_selected_line->head()->time()), getYPixel(m_selected_line->head()->y()));
    }

    if (_selectedLines && _selectedLines->size() > 0) {
        return QPointF(time2Pixel(_selectedLines->head()->time()), getYPixel(_selectedLines->head()->y()));
    }

    return QPointF(0, 0);
}

QPointF Automation::getLastSelectedPoint()
{
    if (m_selected_line && m_selected_line->size() > 0) {
        return QPointF(time2Pixel(m_selected_line->tail()->time()), getYPixel(m_selected_line->tail()->y()));
    }

    if (_selectedLines && _selectedLines->size() > 0) {
        return QPointF(time2Pixel(_selectedLines->tail()->time()), getYPixel(_selectedLines->tail()->y()));
    }

    return QPointF(0, 0);
}

double Automation::getFirstPoint()
{

    if (_cLines.size() > 2) {

        return _cLines.head()->next()->time();
    }

    return 0;
}

double Automation::getLastPoint()
{
    if (_cLines.size() > 2) {

        return _cLines.tail()->prev()->time();
    }

    return 0;
}

Automation::ALine Automation::getLine(int index)
{
    if (index >= 0 && index < _lines.size()) {
        auto value = _lines[index];
        return value;
    }
    return ALine();
}

double Automation::getY(double time)
{

    CPointList* line;
    if (_tLines.size() > 0) {
        line = &_tLines;
    } else {
        line = &_cLines;
    }
    std::vector<CLine> first = find_line_index(line, time);

    if (first.size() == 1) {

        if (first[0].p2() == nullptr) {

            if (first[0].p1()) {

                return first[0].p1()->y();
            }
            return 0;
        } else {
            if (first[0].p2()->isPosInf()) {

                return first[0].p1()->y();
            } else {

                if (first[0].p1() == nullptr) {

                    if (first[0].p2()) {

                        return first[0].p2()->y();
                    }
                }
                if (first[0].p1()->isNegInf()) {

                    return first[0].p2()->y();
                }
            }
        }

        return MathHelper1::yCalculatorFirst(first[0].p1()->pos(), first[0].p2()->pos(), time);
    }
    if (first.size() >= 2) {
        return first[first.size() - 1].p1()->y();
    }

    return 0;
}

bool Automation::contains(SelectedArea selectedArea)
{

    bool case1 = (_trackIndex > selectedArea.firstTrackIndex() && _trackIndex < selectedArea.lastTrackIndex());
    bool case2 = (_trackIndex == selectedArea.firstTrackIndex() && selectedArea.firstTrackIndex() != selectedArea.lastTrackIndex()
        && _subTrackIndex >= selectedArea._firstSubTrackIndex);
    bool case3 = (_trackIndex == selectedArea.lastTrackIndex() && selectedArea.firstTrackIndex() != selectedArea.lastTrackIndex()
        && _subTrackIndex <= selectedArea._lastSubTrackIndex);
    bool case4 = (selectedArea.firstTrackIndex() == _trackIndex && selectedArea.firstTrackIndex() == selectedArea.lastTrackIndex()
        && _subTrackIndex >= selectedArea._firstSubTrackIndex && _subTrackIndex <= selectedArea._lastSubTrackIndex);

    if (case1 || case2 || case3 || case4) {

        return true;
    } else {

        return false;
    }
}

void Automation::selectAll()
{
}

void Automation::clear()
{

    _cLines.clear();
    _tLines.clear();
    _selectedLines->clear();
    clearHoveredLines();
    _copiedLines.clear();

    _cLines.push_back(CPoint::NegInf(_normalized));
    _cLines.push_back(CPoint::PosInf(_normalized));

    Q_EMIT sigLinesChanged();
}

bool Automation::isAutomated()
{
    return _cLines.size() > 2 || _tLines.size() > 2;
}

QVector<QLineF> Automation::selectedLines2Ranegs()
{
    QVector<QLineF> list;

    if (_selected_lines.size() > 0) {
        for (int i = 0; i < _selected_lines.size(); i++) {

            float Y1 = getYPixel(_selected_lines[i].startY);
            float Y2 = getYPixel(_selected_lines[i].endY);
            float X1 = time2Pixel(_selected_lines[i].startTime);
            float X2 = time2Pixel(_selected_lines[i].endTime);

            list.append(QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));
        }
    }

    for (int i = 0; i < list.count(); i++) {
        if (list[i].p1().y() < _windowRect.top()) {
            list[i].setP1(QPointF(list[i].p1().x(), _windowRect.top()));
        }
        if (list[i].p1().y() > _windowRect.bottom()) {
            list[i].setP1(QPointF(list[i].p1().x(), _windowRect.bottom()));
        }

        if (list[i].p2().y() < _windowRect.top()) {
            list[i].setP2(QPointF(list[i].p2().x(), _windowRect.top()));
        }
        if (list[i].p2().y() > _windowRect.bottom()) {
            list[i].setP2(QPointF(list[i].p2().x(), _windowRect.bottom()));
        }
    }

    return list;
}

QVector<QLineF> Automation::list2Ranegs(bool moving)
{
    QVector<QLineF> list;

    if (moving) {

        if (_selected_lines_temp.size() > 0) {

            for (int i = 0; i < _selected_lines_temp.size(); i++) {

                float Y1 = getYPixel(_selected_lines_temp[i].startY);
                float Y2 = getYPixel(_selected_lines_temp[i].endY);
                float X1 = time2Pixel(_selected_lines_temp[i].startTime);
                float X2 = time2Pixel(_selected_lines_temp[i].endTime);

                list.append(QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));
            }

            if (list.size() > 0) {
                float Y1 = getYPixel(_selected_lines_temp.first().startY);
                float Y2 = getYPixel(_selected_lines_temp.first().startY);
                float X1 = -qInf();
                float X2 = time2Pixel(_selected_lines_temp.first().startTime);
                list.insert(0, QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));

                Y1 = getYPixel(_selected_lines_temp[_selected_lines_temp.size() - 1].endY);
                Y2 = getYPixel(_selected_lines_temp[_selected_lines_temp.size() - 1].endY);
                X1 = time2Pixel(_selected_lines_temp[_selected_lines_temp.size() - 1].endTime);
                X2 = qInf();

                list.append(QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));
            } else {
                float X1 = -qInf();
                float X2 = qInf();
                float Y1 = getYPixel(_selected_lines_temp.first().startY);
                float Y2 = getYPixel(_selected_lines_temp.first().startY);
                list.insert(0, QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));
            }

        } else if (_temp_lines.size() > 0) {

            for (int i = 0; i < _temp_lines.size(); i++) {

                float Y1 = getYPixel(_temp_lines[i].startY);
                float Y2 = getYPixel(_temp_lines[i].endY);
                float X1 = time2Pixel(_temp_lines[i].startTime);
                float X2 = time2Pixel(_temp_lines[i].endTime);

                list.append(QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));
            }

            if (list.size() > 0) {
                float Y1 = getYPixel(_temp_lines.first().startY);
                float Y2 = getYPixel(_temp_lines.first().startY);
                float X1 = -qInf();
                float X2 = time2Pixel(_temp_lines.first().startTime);
                list.insert(0, QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));

                Y1 = getYPixel(_temp_lines.last().endY);
                Y2 = getYPixel(_temp_lines.last().endY);
                X1 = time2Pixel(_temp_lines.last().endTime);
                X2 = qInf();

                list.append(QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));

            } else {
                float X1 = -qInf();
                float X2 = qInf();
                float Y1 = getYPixel(_temp_lines.first().startY);
                float Y2 = getYPixel(_temp_lines.first().startY);
                list.insert(0, QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));
            }

        } else if (_lines.size() > 0) {

            for (int i = 0; i < _lines.size(); i++) {

                float Y1 = getYPixel(_lines[i].startY);
                float Y2 = getYPixel(_lines[i].endY);
                float X1 = time2Pixel(_lines[i].startTime);
                float X2 = time2Pixel(_lines[i].endTime);

                list.append(QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));
            }

            if (list.size() > 0) {
                float Y1 = getYPixel(_lines.first().startY);
                float Y2 = getYPixel(_lines.first().startY);
                float X1 = -qInf();
                float X2 = time2Pixel(_lines.first().startTime);
                list.insert(0, QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));

                Y1 = getYPixel(_lines.last().endY);
                Y2 = getYPixel(_lines.last().endY);
                X1 = time2Pixel(_lines.last().endTime);
                X2 = qInf();

                list.append(QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));
            } else {
                float X1 = -qInf();
                float X2 = qInf();
                float Y1 = getYPixel(_lines.first().startY);
                float Y2 = getYPixel(_lines.first().startY);
                list.insert(0, QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));
            }
        }

    } else {
        if (_selected_lines_temp.size() > 0) {

            for (int i = 0; i < _selected_lines_temp.size(); i++) {

                float Y1;
                float Y2;
                Y1 = getYPixel(_selected_lines_temp[i].startY);
                Y2 = getYPixel(_selected_lines_temp[i].endY);

                float X1 = time2Pixel(_selected_lines_temp[i].startTime);
                float X2 = time2Pixel(_selected_lines_temp[i].endTime);

                list.append(QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));
            }

            if (list.size() > 0) {
                float Y1 = getYPixel(_selected_lines_temp.first().startY);
                float Y2 = getYPixel(_selected_lines_temp.first().startY);
                float X1 = -qInf();
                float X2 = time2Pixel(_selected_lines_temp.first().startTime);
                list.insert(0, QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));

                Y1 = getYPixel(_selected_lines_temp[_selected_lines_temp.size() - 1].endY);
                Y2 = getYPixel(_selected_lines_temp[_selected_lines_temp.size() - 1].endY);
                X1 = time2Pixel(_selected_lines_temp[_selected_lines_temp.size() - 1].endTime);
                X2 = qInf();

                list.append(QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));
            } else {
                float X1 = -qInf();
                float X2 = qInf();
                float Y1 = getYPixel(_selected_lines_temp.first().startY);
                float Y2 = getYPixel(_selected_lines_temp.first().startY);
                list.insert(0, QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));
            }

        } else {
            if (_lines.size() > 0) {

                for (int i = 0; i < _lines.size(); i++) {

                    float Y1 = getYPixel(_lines[i].startY);
                    float Y2 = getYPixel(_lines[i].endY);
                    float X1 = time2Pixel(_lines[i].startTime);
                    float X2 = time2Pixel(_lines[i].endTime);

                    list.append(QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));
                }

                if (list.size() > 0) {

                    float Y1 = getYPixel(_lines.first().startY);
                    float Y2 = getYPixel(_lines.first().startY);
                    float X1 = -qInf();
                    float X2 = time2Pixel(_lines.first().startTime);
                    list.insert(0, QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));

                    Y1 = getYPixel(_lines.last().endY);
                    Y2 = getYPixel(_lines.last().endY);
                    X1 = time2Pixel(_lines.last().endTime);
                    X2 = qInf();

                    list.append(QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));

                } else {
                    float X1 = -qInf();
                    float X2 = qInf();
                    float Y1 = getYPixel(_lines.first().startY);
                    float Y2 = getYPixel(_lines.first().startY);
                    list.insert(0, QLineF(QPointF(X1, Y1), QPointF(X2, Y2)));
                }
            }
        }
    }

    return list;
}

QVector<QLineF> Automation::getRanges(bool _mouseMoving)
{
    _ranges = list2Ranegs(_mouseMoving);

    int T = _windowRect.top();
    int B = _windowRect.bottom();
    int R = _windowRect.right();
    int L = _windowRect.left();

    // value = -42.3529 * qPow(Y, 2) - 33.6471 * Y + 6; (value = 0 -> Y = 0.14999988673)

    if (_ranges.count() == 0) {

        // double h = m_norm; // MathHelper1::decibel2yPixel(_trackValue);

        double y = qAbs(_normalized) * (B - T) + T;

        _ranges.append(QLineF(QPointF(-qInf(), y), QPointF(qInf(), y)));
    }

    for (int i = 0; i < _ranges.count(); i++) {

        if (_ranges[i].p1().y() < _windowRect.top()) {
            _ranges[i].p1().setY(_windowRect.top());
        }
        if (_ranges[i].p1().y() > _windowRect.bottom()) {
            _ranges[i].p1().setY(_windowRect.bottom());
        }

        if (_ranges[i].p2().y() < _windowRect.top()) {
            _ranges[i].p2().setY(_windowRect.top());
        }
        if (_ranges[i].p2().y() > _windowRect.bottom()) {
            _ranges[i].p2().setY(_windowRect.bottom());
        }
    }
    return _ranges;
}

void Automation::insertLine(int index, QJsonObject state)
{

    ALine line;
    line.setState(state);
    _lines.insert(index, line);

    Q_EMIT sigLinesChanged();
}

void Automation::editLine(int index, QJsonObject state)
{

    _lines[index].setState(state);
}

void Automation::deleteLine(int index)
{

    if (index >= 0 && index < _lines.size()) {

        _lines.removeAt(index);
    }
}

QJsonArray Automation::getLinesState()
{
    return _cLines.toJsonArray();
}

void Automation::setLinesState(QJsonArray states)
{

    clear();

    _cLines.fromJsonArray(states);

    if (_cLines.size() == 0) {
        _cLines.push_back(CPoint::NegInf(_normalized));
        _cLines.push_back(CPoint::PosInf(_normalized));
    }

    Q_EMIT sigLinesChanged();
}

void Automation::saveTemplateLine()
{

    _selected_lines.clear();
    _selected_line_positions.clear();
    _init_selected_line_move = false;
    if (_temp_lines.size() == 0 && _selected_lines_temp.size() == 0) {

        return;
    }

    _lines.clear();

    if (_selected_lines_temp.size() > 0) {
        for (int i = 0; i < _selected_lines_temp.count(); i++) {
            _lines.append(_selected_lines_temp[i]);
        }
    } else if (_temp_lines.size() > 0) {
        for (int i = 0; i < _temp_lines.count(); i++) {
            _lines.append(_temp_lines[i]);
        }
    }

    for (int i = 0; i < _lines.size(); i++) {
        if (getYPixel(_lines[i].startY) < _windowRect.top()) {
            _lines[i].startY = m_minY;
        } else if (getYPixel(_lines[i].startY) > _windowRect.bottom()) {
            _lines[i].startY = m_maxY;
        }

        if (getYPixel(_lines[i].endY) < _windowRect.top()) {
            _lines[i].endY = m_minY;
        } else if (getYPixel(_lines[i].endY) > _windowRect.bottom()) {
            _lines[i].endY = m_maxY;
        }
    }

    _selected_point_index.clear();

    _temp_lines.clear();
    _selected_lines_temp.clear();

    Q_EMIT sigLinesChanged();
}

int Automation::find_line(double time)
{
    int index = -1;

    if (_lines.size() == 0)
        return -1;

    if (time > _lines.last().endTime)
        return _lines.count();

    if (time < _lines.first().startTime)
        return -1;

    for (index = 0; index < _lines.count(); index++) {
        if (_lines[index].startTime < time && _lines[index].endTime > time) {
            return index;
        }
        if (qAbs(_lines[index].startTime - time) < 0.001) {
            return index;
        }
        if (qAbs(_lines[index].endTime - time) < 0.001) {
            return index;
        }
    }
    // If the time is after the last point, we specify it with this value

    return -1;
}

int Automation::find_line(float startx, float endx)
{
    for (int i = 0; i < _lines.count(); i++) {

        double envelop_point_start_time;
        double envelop_point_end_time;
        envelop_point_start_time = _lines[i].startTime;
        envelop_point_end_time = _lines[i].endTime;

        if (envelop_point_end_time < _areaInfo->startTime())
            continue;

        if (envelop_point_start_time > _areaInfo->startTime() + _areaInfo->clipDuration())
            continue;

        float start_point_pix = time2Pixel(envelop_point_start_time);
        float end_point_pix = time2Pixel(envelop_point_end_time);

        if (start_point_pix < 0) {
            if (end_point_pix == endx) {
                return i;
            }
        } else if (end_point_pix > _areaInfo->lastWidth()) {
            if (startx == start_point_pix) {
                return i;
            }
        } else {
            if (startx == start_point_pix && endx == end_point_pix) {
                return i;
            }
        }
    }
    return -1;
}

int Automation::find_selected_line_start_point(float pixelX, float pixelY)
{

    auto threshold = ScreenInterface::scaleSize2(30, _areaInfo->zoomFactor());

    if (_selected_lines.size() == 0)
        return -1;

    QMap<int, QPointF> list;
    for (int i = 0; i < _selected_lines.count(); i++) {

        double envelop_point_start_time;
        double envelop_point_end_time;

        envelop_point_start_time = _selected_lines[i].startTime;
        envelop_point_end_time = _selected_lines[i].endTime;

        float new_point_pix = time2Pixel(envelop_point_start_time);
        float new_point_pix_end = time2Pixel(envelop_point_end_time);

        if (new_point_pix_end < 0)
            continue;

        if (new_point_pix > _areaInfo->lastWidth())
            continue;

        if (qAbs(pixelX - new_point_pix) < threshold) {
            if (qAbs(pixelY - getYPixel(_selected_lines[i].startY)) < threshold) {
                list.insert(i, QPointF(new_point_pix, _selected_lines[i].startY));
            }
        }
    }

    if (_lines.size() > 0) {

        double envelop_point_time;
        int i = _selected_lines.size() - 1;
        envelop_point_time = _selected_lines[i].endTime;

        float new_point_pix = time2Pixel(envelop_point_time);
        if (qAbs(pixelX - new_point_pix) < threshold) {
            if (qAbs(pixelY - getYPixel(_selected_lines[i].endY)) < threshold) {
                list.insert(i + 1, QPointF(new_point_pix, _selected_lines[i].endY));
            }
        }
    }

    if (list.size() > 0) {
        float min = -1;
        int min_index = 0;
        QMapIterator<int, QPointF> i(list);
        while (i.hasNext()) {
            i.next();
            float dis = MathHelper1::distance(i.value().x(), pixelX, i.value().y(), pixelY);

            if (min < 0) {
                min = dis;
                min_index = i.key();
            } else {
                if (dis < min && qAbs(min - dis) > 0.001) {
                    dis = min;
                    min_index = i.key();
                }
            }
        }

        return min_index;
    } else {

        return -1;
    }
}

int Automation::find_line_start_point(float pixelX, float pixelY)
{

    auto threshold = ScreenInterface::scaleSize2(30, _areaInfo->zoomFactor());

    QMap<int, QPointF> list;
    for (int i = 0; i < _lines.count(); i++) {

        double envelop_point_start_time;
        double envelop_point_end_time;

        envelop_point_start_time = _lines[i].startTime;
        envelop_point_end_time = _lines[i].endTime;

        float new_point_pix = time2Pixel(envelop_point_start_time);
        float new_point_pix_end = time2Pixel(envelop_point_end_time);

        if (new_point_pix_end < 0)
            continue;

        if (new_point_pix > _areaInfo->lastWidth())
            continue;

        if (qAbs(pixelX - new_point_pix) < threshold) {
            if (qAbs(pixelY - getYPixel(_lines[i].startY)) < threshold) {
                list.insert(i, QPointF(new_point_pix, _lines[i].startY));
            }
        }
    }

    if (_lines.size() > 0) {

        double envelop_point_time;
        int i = _lines.size() - 1;
        envelop_point_time = _lines[i].endTime;

        float new_point_pix = time2Pixel(envelop_point_time);
        if (qAbs(pixelX - new_point_pix) < threshold) {
            if (qAbs(pixelY - getYPixel(_lines[i].endY)) < threshold) {
                list.insert(i + 1, QPointF(new_point_pix, _lines[i].endY));
            }
        }
    }

    if (list.size() > 0) {
        float min = -1;
        int min_index = 0;
        QMapIterator<int, QPointF> i(list);
        while (i.hasNext()) {
            i.next();
            double dis = MathHelper1::distance(i.value().x(), pixelX, i.value().y(), pixelY);

            if (min < 0) {
                min = dis;
                min_index = i.key();

            } else {

                if (dis < min && qAbs(min - dis) > 0.001) {
                    min = dis;
                    min_index = i.key();
                }
            }
        }

        return min_index;
    } else {

        return -1;
    }
}

int Automation::find_line(float SpixelX, float SpixelY, float EpixelX, float EpixelY)
{
    auto threshold = ScreenInterface::scaleSize2(30, _areaInfo->zoomFactor());

    QMap<int, QPointF> list;
    for (int i = 0; i < _lines.count(); i++) {

        double envelop_point_start_time;
        double envelop_point_end_time;

        envelop_point_start_time = _lines[i].startTime;
        envelop_point_end_time = _lines[i].endTime;

        float new_point_pix_start = time2Pixel(envelop_point_start_time);
        float new_point_pix_end = time2Pixel(envelop_point_end_time);

        if (new_point_pix_end < 0)
            continue;

        if (new_point_pix_start > _areaInfo->lastWidth())
            continue;

        auto y0 = getYPixel(_lines[i].startY);
        auto y1 = getYPixel(_lines[i].endY);

        // qDebug() << "YYY" << y0 << y1 << SpixelY << EpixelY;

        if (qAbs(SpixelX - new_point_pix_start) < threshold) {
            if (qAbs(SpixelY - y0) < threshold) {
                if (qAbs(EpixelX - new_point_pix_end) < threshold) {
                    if (qAbs(EpixelY - y1) < threshold) {
                        return i;
                    }
                }
            }
        }
    }
    return -1;
}

int Automation::findSelected_point(float SpixelX, float SpixelY, float EpixelX, float EpixelY)
{

    auto threshold = ScreenInterface::scaleSize2(30, _areaInfo->zoomFactor());

    QMap<int, QPointF> list;
    for (int i = 0; i < _selected_lines_temp.count(); i++) {

        double envelop_point_start_time;
        double envelop_point_end_time;

        envelop_point_start_time = _selected_lines_temp[i].startTime;
        envelop_point_end_time = _selected_lines_temp[i].endTime;

        float new_point_pix = time2Pixel(envelop_point_start_time);
        float new_point_pix_end = time2Pixel(envelop_point_end_time);

        if (new_point_pix_end < 0)
            continue;

        if (new_point_pix > _areaInfo->lastWidth())
            continue;

        if (qAbs(SpixelX - new_point_pix) < threshold) {
            if (qAbs(SpixelY - getYPixel(_selected_lines_temp[i].startY)) < threshold) {
                if (qAbs(EpixelX - new_point_pix_end) < threshold) {
                    if (qAbs(EpixelY - getYPixel(_selected_lines_temp[i].endY)) < threshold) {
                        return i;
                    }
                }
            }
        }
    }
    return -1;
}

int Automation::find_first_selected_lines_index(double time)
{
    QMap<int, QPointF> list;
    if (time < _lines.first().startTime) {
        return -1;
    }
    if (time >= _lines.last().endTime) {
        return _lines.size();
    }
    int index = -1;
    for (int i = 0; i < _lines.count(); i++) {

        double envelop_point_start_time;
        double envelop_point_end_time;

        envelop_point_start_time = _lines[i].startTime;
        envelop_point_end_time = _lines[i].endTime;

        if (time > envelop_point_start_time && time < envelop_point_end_time && time != envelop_point_end_time) {
            index = i;

        } else if (qAbs(time - envelop_point_start_time) < 0.0001) {
            index = i;
        }
    }

    return index;
}

int Automation::find_first_first_list_index(double time)
{
    QMap<int, QPointF> list;

    if (_first_list.size() == 0)
        return -1;

    if (time < _first_list.first().startTime) {
        return -1;
    }
    if (time >= _start_line.startTime) {
        return _first_list.size();
    }

    int index = -1;
    for (int i = 0; i < _first_list.count(); i++) {

        double envelop_point_start_time;
        double envelop_point_end_time;

        envelop_point_start_time = _first_list[i].startTime;
        envelop_point_end_time = _first_list[i].endTime;

        if (time > envelop_point_start_time && time <= envelop_point_end_time && time != envelop_point_end_time) {
            index = i;
        } else if (qAbs(time - envelop_point_start_time) < 0.0001) {
            index = i;
        }
    }
    return index;
}

int Automation::find_last_selected_lines_index(double time)
{
    QMap<int, QPointF> list;
    if (time < _lines.first().startTime) {
        return -1;
    }
    if (time > _lines.last().endTime) {
        return _lines.size();
    }
    int index = -1;
    for (int i = 0; i < _lines.count(); i++) {

        double envelop_point_start_time;
        double envelop_point_end_time;

        envelop_point_start_time = _lines[i].startTime;
        envelop_point_end_time = _lines[i].endTime;

        if (time > envelop_point_start_time && time < envelop_point_end_time && time != envelop_point_start_time) {
            index = i;
            break;
        } else if (qAbs(envelop_point_end_time - time) < 0.0001) {
            if (i == 0) {
                if (qAbs(envelop_point_start_time - envelop_point_end_time) > 0.0001) {
                    index = i;
                    break;
                }
            } else {
                index = i;
                break;
            }
        }
    }
    return index;
}

int Automation::find_last_last_list_index(double time)
{
    QMap<int, QPointF> list;

    if (_last_list.size() == 0)
        return _lines.size();

    if (time < _end_line.endTime) {
        return -1;
    }
    if (time > _last_list.last().endTime) {
        return _last_list.size();
    }
    int index = -1;
    for (int i = 0; i < _last_list.count(); i++) {

        double envelop_point_start_time;
        double envelop_point_end_time;

        envelop_point_start_time = _last_list[i].startTime;
        envelop_point_end_time = _last_list[i].endTime;

        if (time > envelop_point_start_time && time < envelop_point_end_time && time != envelop_point_start_time) {
            index = i;
            break;
        } else if (qAbs(envelop_point_end_time - time) < 0.0001) {
            if (i == 0) {
                if (qAbs(envelop_point_start_time - envelop_point_end_time) > 0.0001) {
                    index = i;
                    break;
                }
            } else {
                index = i;
                break;
            }
        }
    }
    return index;
}

double Automation::getStartY(double time, int index)
{

    double a = (_lines[index].endY - _lines[index].startY) / (_lines[index].endTime - _lines[index].startTime);
    double y = a * (time - _lines[index].startTime) + _lines[index].startY;

    return y;
}
double Automation::getStartTime(float y, QVector<ALine> selected_lines, int index)
{

    double a = (selected_lines[index].endY - selected_lines[index].startY) / (selected_lines[index].endTime - selected_lines[index].startTime);
    if (a == qInf()) {
        return selected_lines[index].startTime;
    }
    double time = (y - selected_lines[index].startY) / a + selected_lines[index].startTime;
    return time;
}

float Automation::find_first_selected_lines_Y(QVector<ALine> selected_lines, int index)
{
    double sY = 0;
    if (index < 0) {
        sY = _lines.first().startY;
    } else if (index >= _lines.size()) {
        sY = _lines.last().endY;
    } else {
        sY = getStartY(selected_lines.first().startTime, index);
        if (qIsNaN(sY)) {
            return _lines[index].startY;
        }
    }
    return sY;
}

float Automation::find_last_selected_lines_Y(QVector<ALine> selected_lines, int index)
{
    float eY = 0;
    if (index < 0) {
        eY = _lines.first().startY;
    } else if (index >= _lines.size()) {
        eY = _lines.last().endY;
    } else {
        eY = getStartY(selected_lines.last().endTime, index);

        if (qIsNaN(eY)) {
            return _lines[index].endY;
        }
    }
    return eY;
}

float Automation::find_first_first_lines_Y(int index)
{
    float sY = 0;
    if (index < 0) {
        sY = _lines.first().startY;
    } else if (index >= _first_list.size()) {
        sY = _lines.last().endY;
    } else {
        //   sY = getStartY(first_list.first().startTime, index);

        float a = (_first_list[index].endY - _first_list[index].startY) / (_first_list[index].endTime - _first_list[index].startTime);
        sY = a * (_selected_lines.first().startTime - _first_list[index].startTime) + _first_list[index].startY;
    }
    return sY;
}

float Automation::find_last_last_lines_Y(int index)
{
    float eY = 0;
    if (index < 0) {
        eY = _lines.first().startY;
    } else if (index >= _last_list.size()) {
        eY = _lines.last().endY;
    } else {
        // eY = getStartY(last_list.last().endTime, index);

        float a = (_last_list[index].endY - _last_list[index].startY) / (_last_list[index].endTime - _last_list[index].startTime);
        eY = a * (_selected_lines.last().endTime - _last_list[index].startTime) + _last_list[index].startY;
    }
    return eY;
}

float Automation::find_lines_Y(QVector<ALine> lines, double time, int index)
{
    float sY = 0;
    if (index < 0) {
        sY = _lines.first().startY;
    } else if (index >= lines.size()) {

        if (lines.size() > 0)
            sY = lines.last().endY;
        else
            sY = _lines.last().endY;
    } else {

        float a = (lines[index].endY - lines[index].startY) / (lines[index].endTime - lines[index].startTime);
        sY = a * (time - lines[index].startTime) + lines[index].startY;
    }
    return sY;
}

bool Automation::getSelectedRangeMoveEnabled() const
{
    return _selectedRangeMoveEnabled;
}

void Automation::setSelectedRangeMoveEnabled(bool selectedRangeMoveEnabled)
{
    _selectedRangeMoveEnabled = selectedRangeMoveEnabled;
}

void Automation::createSnapLine(bool pointIncluded, QPointF hitPoint) // createAutomationPositionList
{

    if (_areaInfo->stepPlotDuration() == 0.0) {
        return;
    }

    double time = _areaInfo->stepPlotDuration();

    double start = 0;
    double end = qCeil((_areaInfo->startTime() + _areaInfo->clipDuration()) / time) * time;

    QMap<double, bool> map;

    while (start <= end) {
        double xpos = time2Pixel_warpLess(start);
        map.insert(xpos, false);
        start += _areaInfo->stepPlotDuration();
    }

    double xpos = time2Pixel(start);
    map.insert(xpos, false);

    if (pointIncluded) {
        _lastSnapAutomationPoints.clear();
        auto points = _cLines.toList();

        for (std::size_t i = 1; i < points.size(); ++i) {
            CPoint* a = points[i - 1];
            CPoint* b = points[i];

            double t0 = a->time();
            double t1 = b->time();

            _lastSnapAutomationPoints.append(Snap(time2Pixel(t0), time2Pixel(t1)));
        }
    }

    for (int j = 0; j < _lastSnapAutomationPoints.size(); j++) {

        double X1 = _lastSnapAutomationPoints[j].first;
        double X2 = _lastSnapAutomationPoints[j].second;
        if (X1 >= 0 && X1 <= _areaInfo->lastWidth())
            map.insert(X1, -1);
        if (X2 >= 0 && X2 <= _areaInfo->lastWidth())
            map.insert(X2, -1);
    }

    map.insert(hitPoint.x(), false);

    QList<double> points;
    QMapIterator<double, bool> i(map);
    while (i.hasNext()) {
        i.next();

        points.append(i.key());
    }

    _snapPoints.clear();
    for (int i = 0; i < points.count() - 1; i++) {
        _snapPoints.append(Snap(points[i], points[i + 1]));
    }
}

void Automation::createSnapLine(bool pointIncluded) // createAutomationPositionList
{

    if (_areaInfo->stepPlotDuration() == 0.0) {
        return;
    }

    double time = _areaInfo->stepPlotDuration();

    double start = 0;
    double end = qCeil((_areaInfo->startTime() + _areaInfo->clipDuration()) / time) * time;

    QMap<double, bool> map;

    while (start <= end) {
        double xpos = time2Pixel(start);
        map.insert(xpos, false);
        start += _areaInfo->stepPlotDuration();
    }

    double xpos = time2Pixel(start);
    map.insert(xpos, false);

    if (pointIncluded) {
        _lastSnapAutomationPoints.clear();
        auto points = _cLines.toList();

        for (std::size_t i = 1; i < points.size(); ++i) {
            CPoint* a = points[i - 1];
            CPoint* b = points[i];

            double t0 = a->time();
            double t1 = b->time();

            _lastSnapAutomationPoints.append(Snap(time2Pixel(t0), time2Pixel(t1)));
        }
    }

    for (int j = 0; j < _lastSnapAutomationPoints.size(); j++) {

        double X1 = _lastSnapAutomationPoints[j].first;
        double X2 = _lastSnapAutomationPoints[j].second;

        if (X1 >= 0 && X1 <= _areaInfo->lastWidth())
            map.insert(X1, -1);
        if (X2 >= 0 && X2 <= _areaInfo->lastWidth())
            map.insert(X2, -1);
    }

    QList<double> points;
    QMapIterator<double, bool> i(map);
    while (i.hasNext()) {
        i.next();

        points.append(i.key());
    }

    _snapPoints.clear();
    for (int i = 0; i < points.count() - 1; i++) {
        _snapPoints.append(Snap(points[i], points[i + 1]));
    }
}

void Automation::createSnapLine(bool pointIncluded, int excludePoint1, int excludePoint2)
{
    if (_areaInfo->stepPlotDuration() == 0.0) {
        return;
    }

    double time = _areaInfo->stepPlotDuration();

    double start = 0;
    double end = qCeil((_areaInfo->startTime() + _areaInfo->clipDuration()) / time) * time;

    QMap<double, bool> map;

    while (start <= end) {
        double xpos = time2Pixel_warpLess(start);
        map.insert(xpos, false);
        start += _areaInfo->stepPlotDuration();
    }

    double xpos = time2Pixel(start);
    map.insert(xpos, false);

    if (pointIncluded) {
        _lastSnapAutomationPoints.clear();
        for (int j = 0; j < _lines.size(); j++) {

            if (j == excludePoint1) {
                continue;
            }

            if (j == excludePoint2) {
                continue;
            }

            double e = _lines[j].endTime;
            double s = _lines[j].startTime;

            _lastSnapAutomationPoints.append(Snap(s, e));
        }
    }

    for (int j = 0; j < _lastSnapAutomationPoints.size(); j++) {

        double X1 = time2Pixel(_lastSnapAutomationPoints[j].first);
        double X2 = time2Pixel(_lastSnapAutomationPoints[j].second);

        if (X1 >= 0 && X1 <= _areaInfo->lastWidth())
            map.insert(X1, -1);
        if (X2 >= 0 && X2 <= _areaInfo->lastWidth())
            map.insert(X2, -1);
    }

    QList<double> points;
    QMapIterator<double, bool> i(map);
    while (i.hasNext()) {
        i.next();

        points.append(i.key());
    }

    _snapPoints.clear();
    for (int i = 0; i < points.count() - 1; i++) {
        _snapPoints.append(Snap(points[i], points[i + 1]));
    }
}

void Automation::findSnap(double posX, double posY, double& final_xpos, double& final_ypos)
{
    Q_UNUSED(posY);
    final_ypos = posY;

    if (_areaInfo->stepPlotDuration() == 0.0 || _snapPoints.isEmpty()) {
        final_xpos = posX;
        return;
    }

    // Binary search by first element of the pair
    auto it = std::lower_bound(
        _snapPoints.begin(),
        _snapPoints.end(),
        posX,
        [](const Snap& s, double x) { return s.first < x; });

    // Case 1: posX is before the first interval
    if (it == _snapPoints.begin()) {
        const Snap& s = _snapPoints.first();
        if (posX <= s.first)
            final_xpos = s.first;
        else if (posX <= s.second)
            final_xpos = (posX < (s.first + s.second) * 0.5 ? s.first : s.second);
        else
            final_xpos = s.second;
        return;
    }

    // Case 2: posX is after the last interval
    if (it == _snapPoints.end()) {
        final_xpos = _snapPoints.last().second;
        return;
    }

    // Now we have:
    //   prev = interval whose .first < posX
    //   it   = first interval whose .first >= posX
    const Snap& next = *it;
    const Snap& prev = *(it - 1);

    // Check if posX is inside previous interval
    if (posX >= prev.first && posX <= prev.second) {
        double mid = (prev.first + prev.second) * 0.5;
        final_xpos = (posX < mid ? prev.first : prev.second);
        return;
    }

    // Otherwise snap to the closest boundary between prev.second and next.first
    double d1 = std::abs(posX - prev.second);
    double d2 = std::abs(posX - next.first);

    final_xpos = (d1 < d2 ? prev.second : next.first);
}

// threshold: maximum distance (in X units) at which snapping occurs.
// If posX is farther than threshold from all snap boundaries, final_xpos = posX.
void Automation::findSnapWithThreshold(double posX,
    double posY,
    double& final_xpos,
    double& final_ypos,
    double threshold)
{
    final_ypos = posY;

    if (threshold < 0.0)
        threshold = 0.0;

    if (_areaInfo->stepPlotDuration() == 0.0 || _snapPoints.isEmpty()) {
        final_xpos = posX;
        return;
    }

    // Binary search by first element of the pair
    auto it = std::lower_bound(
        _snapPoints.begin(),
        _snapPoints.end(),
        posX,
        [](const Snap& s, double x) { return s.first < x; });

    auto snapIfClose = [&](double candidate) -> bool {
        if (std::abs(posX - candidate) <= threshold) {
            final_xpos = candidate;
            return true;
        }
        return false;
    };

    // Case 1: posX is before the first interval
    if (it == _snapPoints.begin()) {
        const Snap& s = _snapPoints.first();

        // Candidates are the boundaries (first/second)
        // Snap only if within threshold; otherwise move freely.
        if (snapIfClose(s.first))
            return;
        if (snapIfClose(s.second))
            return;

        final_xpos = posX;
        return;
    }

    // Case 2: posX is after the last interval
    if (it == _snapPoints.end()) {
        const Snap& s = _snapPoints.last();

        if (snapIfClose(s.second))
            return;
        if (snapIfClose(s.first))
            return;

        final_xpos = posX;
        return;
    }

    // Now we have:
    //   prev = interval whose .first < posX
    //   it   = first interval whose .first >= posX
    const Snap& next = *it;
    const Snap& prev = *(it - 1);

    // Consider the nearest relevant boundaries around posX:
    // - prev.second (end of previous interval)
    // - next.first  (start of next interval)
    // Also, if posX is inside prev interval, boundaries are prev.first and prev.second.
    if (posX >= prev.first && posX <= prev.second) {
        // Inside an interval: snap to nearest edge only if within threshold.
        // Otherwise move freely inside.
        double dLeft = std::abs(posX - prev.first);
        double dRight = std::abs(posX - prev.second);

        double closest = (dLeft <= dRight ? prev.first : prev.second);
        if (std::min(dLeft, dRight) <= threshold) {
            final_xpos = closest;
        } else {
            final_xpos = posX;
        }
        return;
    }

    // Between prev and next: snap to whichever boundary is closer, but only if within threshold.
    double dToPrevEnd = std::abs(posX - prev.second);
    double dToNextStart = std::abs(posX - next.first);

    double closest = (dToPrevEnd <= dToNextStart ? prev.second : next.first);
    if (std::min(dToPrevEnd, dToNextStart) <= threshold) {
        final_xpos = closest;
    } else {
        final_xpos = posX;
    }
}

double Automation::time2Pixel(double time)
{

    if (std::isinf(time) && std::signbit(time)) {

        return 0;
    } else if (std::isinf(time) && !std::signbit(time)) {
        return _areaInfo->lastWidth();
    }

    return _areaInfo->time2Pixel(time);
}

double Automation::pixel2Time(double pixel)
{

    return _areaInfo->pixel2Time(pixel);
}

double Automation::time2Pixel_warpLess(double time)
{
    return ((time - _areaInfo->startTime()) / (double)_areaInfo->clipDuration() * _areaInfo->lastWidth());
}

double Automation::pixel2Time_warpLess(double pixel)
{
    return _areaInfo->startTime() + pixel / (double)_areaInfo->lastWidth() * _areaInfo->clipDuration();
}

double Automation::width2Time(double time, double width)
{
    return _areaInfo->startTime() + width / (double)_areaInfo->lastWidth() * _areaInfo->clipDuration();
}

int Automation::findRangeIndex(float SposX, float SposY, float EposX, float EposY, bool moving)
{

    _ranges = getRanges(moving);

    for (int i = 0; i < _ranges.count(); i++) {

        if (qAbs(_ranges[i].p1().x() - SposX) <= 6) {
            if (qAbs(_ranges[i].p1().y() - SposY) <= 6) {

                if (i == _ranges.count() - 1) {
                    return i;
                } else {
                    if (qAbs(_ranges[i].p2().x() - EposX) <= 6)
                        if (qAbs(_ranges[i].p2().y() - EposY) <= 6)
                            return i;
                }
            }
        }

        if (qAbs(_ranges[i].p2().x() - EposX) <= 6) {
            if (qAbs(_ranges[i].p2().y() - EposY) <= 6) {

                if (i == 0) {
                    return i;
                } else {
                    if (qAbs(_ranges[i].p1().x() - SposX) <= 6)
                        if (qAbs(_ranges[i].p1().y() - SposY) <= 6)
                            return i;
                }
            }
        }
    }
    return -1;
}

int Automation::findRangeIndex(float posX)
{

    _ranges = getRanges(false);
    for (int i = 0; i < _ranges.count(); i++) {

        float pos = posX;
        float start = _ranges[i].p1().x();
        float end = _ranges[i].p2().x();
        if (pos > start && pos < end) {
            return i;
        }

        if (qAbs(pos - start) < 1) {
            return i;
        }

        if (qAbs(pos - start) < 1 && qAbs(pos - end) < 1) {
            return i;
        }
        if (qAbs(pos - end) < 1) {
            return i + 1;
        }
    }

    return -1;
}

int Automation::findRangeIndex(float posX, float posY)
{

    _ranges = getRanges(false);

    for (int i = 0; i < _ranges.count(); i++) {

        if (qAbs(_ranges[i].p2().x() - posX) <= 6) {
            if (qAbs(_ranges[i].p2().y() - posY) <= 6) {

                return i;
            }
        }
    }
    return -1;
}

bool Automation::findNewRoot(int index, double time, float& p, float& q, float& r, float& s, float& y)
{

    double curve_n = 1;
    double curve_m = 1;

    if (index < 0 || index >= _lines.size()) {
        return false;
    }

    curve_n = _lines[index].curve_n;
    curve_m = _lines[index].curve_m;

    if (curve_n == 1.0 && curve_m == 1.0)
        return false;

    double cutX = time2Pixel(time);

    QPointF a = QPointF(time2Pixel(_lines[index].endTime), getYPixel(_lines[index].endY));
    QPointF b = QPointF(time2Pixel(_lines[index].startTime), getYPixel(_lines[index].startY));

    double A, B, centerX, centerY;
    if (curve_n < 0) {
        centerX = a.x();
        centerY = b.y();

        QPointF bb = QPointF(b.x(), b.y());
        QPointF aa = QPointF(a.x(), a.y());

        B = -(aa.y() - centerY);
        A = -(centerX - bb.x());

    } else {

        centerX = b.x();
        centerY = a.y();

        QPointF bb = QPointF(b.x(), b.y());
        QPointF aa = QPointF(a.x(), a.y());

        A = (aa.x() - centerX);
        B = (centerY - bb.y());
    }

    auto result = MathHelper1::superEllipseXtoY(a, b, curve_n, curve_m,
        cutX);

    double x = result.x();
    y = result.y();

    if (qAbs(centerY - y) < 0.0001) {
        return false;
    }
    if (qAbs(x - centerX) < 0.0001) {
        return false;
    }
    MathHelper1::findRoot(qAbs(curve_n), qAbs(curve_m), 0, 0, qAbs(A), qAbs(B), 1000, x - centerX, centerY - y, p, q, r, s);

    if (curve_n < 0) {
        p = -p;
        q = -q;
        r = -r;
        s = -s;
    }

    y = (result.y() - _windowRect.top()) / (double)(_windowRect.bottom() - _windowRect.top());
    return true;
}

bool Automation::findNewRoot(QVector<Automation::ALine> lines, int index, double time, float& p, float& q, float& r, float& s, float& y)
{

    double curve_n = 1;
    double curve_m = 1;

    if (index < 0 || index >= lines.size()) {
        return false;
    }

    curve_n = lines[index].curve_n;
    curve_m = lines[index].curve_m;

    if (curve_n == 1.0 && curve_m == 1.0)
        return false;

    double cutX = time2Pixel(time);

    QPointF a = QPointF(time2Pixel(lines[index].endTime), getYPixel(lines[index].endY));
    QPointF b = QPointF(time2Pixel(lines[index].startTime), getYPixel(lines[index].startY));

    double A, B, centerX, centerY;
    if (curve_n < 0) {
        centerX = a.x();
        centerY = b.y();

        QPointF bb = QPointF(b.x(), b.y());
        QPointF aa = QPointF(a.x(), a.y());

        B = -(aa.y() - centerY);
        A = -(centerX - bb.x());

    } else {

        centerX = b.x();
        centerY = a.y();

        QPointF bb = QPointF(b.x(), b.y());
        QPointF aa = QPointF(a.x(), a.y());

        A = (aa.x() - centerX);
        B = (centerY - bb.y());
    }

    if (qAbs(cutX - centerX) < 0.0001) {
        return false;
    }

    auto result = MathHelper1::superEllipseXtoY(a, b, curve_n, curve_m,
        cutX);

    double x = result.x();
    y = result.y();

    if (qAbs(x - centerX) < 0.0001) {
        return false;
    }

    if (qAbs(centerY - y) < 0.0001) {
        return false;
    }

    MathHelper1::findRoot(qAbs(curve_n), qAbs(curve_m), 0, 0, qAbs(A), qAbs(B), 1000, x - centerX, centerY - y, p, q, r, s);

    if (curve_n < 0) {
        p = -p;
        q = -q;
        r = -r;
        s = -s;
    }
    y = (result.y() - _windowRect.top()) / (double)(_windowRect.bottom() - _windowRect.top());
    return true;
}

void Automation::createSelectedPoints(double startMarker, double endMarker)
{

    _topMarker = _areaInfo->selectionArea()->area().top();
    _bottomMarker = _areaInfo->selectionArea()->area().bottom();
    auto selectedArea = _areaInfo->selectionArea();

    if ((_trackIndex > selectedArea->firstTrackIndex() && _trackIndex < selectedArea->lastTrackIndex())
        || (_trackIndex == selectedArea->firstTrackIndex() && selectedArea->firstTrackIndex() != selectedArea->lastTrackIndex() && getSubTrackIndex() >= selectedArea->_firstSubTrackIndex)
        || (_trackIndex == selectedArea->lastTrackIndex() && selectedArea->firstTrackIndex() != selectedArea->lastTrackIndex() && getSubTrackIndex() <= selectedArea->_lastSubTrackIndex)
        || (selectedArea->firstTrackIndex() == _trackIndex && selectedArea->firstTrackIndex() == selectedArea->lastTrackIndex() && getSubTrackIndex() >= selectedArea->_firstSubTrackIndex
            && getSubTrackIndex() <= selectedArea->_lastSubTrackIndex)) {

    } else {

        return;
    }

    if (_areaInfo->selectionArea()->isClipSelectedAtFirst == true || _areaInfo->selectionArea()->isClipSelectedAtLast) {

        return;
    }

    if (qAbs(startMarker - endMarker) <= 0.0) {

        clearSelectedLines();
        return;
    }

    if (_lines.size() == 0) {

        _selected_lines.clear();
        return;
    }

    double startPix = time2Pixel(startMarker);
    double endPix = time2Pixel(endMarker);

    int sIndex = findRangeIndex(startPix);
    int eIndex = findRangeIndex(endPix);
    double sY = MathHelper1::yCalculatorFirst(_ranges[sIndex].p1(), _ranges[sIndex].p2(), startPix);
    double eY = MathHelper1::yCalculatorLast(_ranges[eIndex].p1(), _ranges[eIndex].p2(), endPix);

    sY = (sY - _windowRect.top()) / (double)(_windowRect.bottom() - _windowRect.top());
    eY = (eY - _windowRect.top()) / (double)(_windowRect.bottom() - _windowRect.top());

    double sTime = startMarker;
    double eTime = endMarker;

    int startIndex = -1;
    int endIndex = -1;
    for (int i = 0; i < _lines.count(); i++) {

        double start_time = _lines[i].startTime;
        double end_time = _lines[i].endTime;

        if (start_time < sTime && end_time > sTime) {
            startIndex = i;
            break;
        }
        if (qAbs(start_time - sTime) <= 0.001) {
            startIndex = i;
            break;
        }
        if (qAbs(start_time - sTime) <= 0.001 && qAbs(end_time - sTime) <= 0.001) {
            startIndex = i;
            break;
        }
    }

    for (int i = 0; i < _lines.count(); i++) {

        double start_time = _lines[i].startTime;
        double end_time = _lines[i].endTime;

        if (start_time < eTime && end_time > eTime) {
            endIndex = i;
        }

        if (qAbs(end_time - eTime) <= 0.001) {
            endIndex = i;
        }

        if (qAbs(start_time - eTime) <= 0.001 && qAbs(end_time - eTime) <= 0.001) {
            endIndex = i;
        }
    }

    if (sTime < _lines.first().startTime) {
        startIndex = -1;
    }
    if (sTime > _lines.last().endTime) {
        startIndex = _lines.count();
    }
    if (eTime < _lines.first().startTime) {
        endIndex = -1;
    }
    if (eTime > _lines.last().endTime) {
        endIndex = _lines.count();
    }

    if (startIndex >= 0 && startIndex < _lines.size() - 1 && qAbs(sY - 0) <= 0) {
        sY = _lines[startIndex].startY;
    }
    if (endIndex >= 0 && endIndex < _lines.size() - 1 && qAbs(eY - 0) <= 0) {
        eY = _lines[endIndex].endY;
    }

    if (startIndex > endIndex) {
        qDebug() << "BUG!2";
        return;
    }

    if ((startIndex >= 0 && startIndex < _lines.size()) && (endIndex >= 0 && endIndex < _lines.size()) && startIndex != endIndex) {

        qDebug() << "createSelectedPoints:862";
        _init_selected_line_move = true;

        _selected_lines.clear();
        _selected_line_positions.clear();

        float p, q, r, s, y;
        bool result = findNewRoot(startIndex, sTime, p, q, r, s, y);

        if (result) {
            _selected_lines.append((ALine { sTime, _lines[startIndex].endTime, y, _lines[startIndex].endY, r, s }));
        } else {
            _selected_lines.append((ALine { sTime, _lines[startIndex].endTime, sY, _lines[startIndex].endY, _lines[startIndex].curve_n, _lines[startIndex].curve_m }));
        }

        for (int i = startIndex + 1; i <= endIndex - 1; i++) {
            _selected_lines.append(ALine { _lines[i].startTime, _lines[i].endTime, _lines[i].startY, _lines[i].endY, _lines[i].curve_n, _lines[i].curve_m });
        }

        result = findNewRoot(endIndex, eTime, p, q, r, s, y);

        qDebug() << "createSelectedPoints:862" << endIndex << eTime << p << q << r << s << y << result;
        if (result) {
            _selected_lines.append((ALine { _lines[endIndex].startTime, eTime, _lines[endIndex].startY, y, p, q }));
        } else {
            _selected_lines.append((ALine { _lines[endIndex].startTime, eTime, _lines[endIndex].startY, eY, _lines[endIndex].curve_n, _lines[endIndex].curve_m }));
        }
    }

    if ((startIndex >= 0 && startIndex < _lines.size()) && (endIndex >= 0 && endIndex < _lines.size()) && startIndex == endIndex) {

        _init_selected_line_move = true;

        _selected_lines.clear();
        _selected_line_positions.clear();

        _selected_lines.append((ALine { sTime, eTime, sY, eY, 1 }));
    }

    if (startIndex < 0 && (endIndex >= 0 && endIndex < _lines.size()) && startIndex != endIndex) {

        _init_selected_line_move = true;

        _selected_lines.clear();
        _selected_line_positions.clear();

        _selected_lines.append((ALine { sTime, _lines.first().startTime, sY, _lines.first().startY, 1 }));
        for (int i = 0; i <= endIndex - 1; i++) {
            _selected_lines.append(ALine { _lines[i].startTime, _lines[i].endTime, _lines[i].startY, _lines[i].endY, _lines[i].curve_n, _lines[i].curve_m });
        }

        float p, q, r, s, y;
        bool result = findNewRoot(endIndex, eTime, p, q, r, s, y);

        if (result) {
            _selected_lines.append((ALine { _lines[endIndex].startTime, eTime, _lines[endIndex].startY, y, p, q }));
        } else {
            _selected_lines.append((ALine { _lines[endIndex].startTime, eTime, _lines[endIndex].startY, eY, _lines[endIndex].curve_n, _lines[endIndex].curve_m }));
        }
    }

    if ((startIndex >= 0 && startIndex < _lines.size()) && endIndex >= _lines.size() && startIndex != endIndex) {

        _init_selected_line_move = true;

        _selected_lines.clear();
        _selected_line_positions.clear();

        float p, q, r, s, y;
        bool result = findNewRoot(startIndex, sTime, p, q, r, s, y);

        if (result) {
            _selected_lines.append((ALine { sTime, _lines[startIndex].endTime, y, _lines[startIndex].endY, r, s }));

        } else {
            _selected_lines.append((ALine { sTime, _lines[startIndex].endTime, sY, _lines[startIndex].endY, _lines[startIndex].curve_n, _lines[startIndex].curve_m }));
        }

        for (int i = startIndex + 1; i < _lines.size(); i++) {
            _selected_lines.append(ALine { _lines[i].startTime, _lines[i].endTime, _lines[i].startY, _lines[i].endY, _lines[i].curve_n, _lines[i].curve_m });
        }

        _selected_lines.append((ALine { _lines.last().endTime, eTime, _lines.last().endY, eY, 1 }));
    }

    if (startIndex < 0 && endIndex < 0) {
        _init_selected_line_move = true;

        _selected_lines.clear();
        _selected_line_positions.clear();

        _selected_lines.append((ALine { sTime, eTime, sY, eY, 1 }));
    }

    if (startIndex >= _lines.size() && endIndex >= _lines.size()) {
        _init_selected_line_move = true;

        _selected_lines.clear();
        _selected_line_positions.clear();

        _selected_lines.append((ALine { sTime, eTime, sY, eY, 1 }));
    }

    if (startIndex < 0 && endIndex >= _lines.size()) {
        _init_selected_line_move = true;

        _selected_lines.clear();
        _selected_line_positions.clear();

        _selected_lines.append((ALine { sTime, _lines.first().startTime, sY, _lines.first().startY, 1 }));
        for (int i = 0; i < _lines.size(); i++) {
            _selected_lines.append(ALine { _lines[i].startTime, _lines[i].endTime, _lines[i].startY, _lines[i].endY, _lines[i].curve_n, _lines[i].curve_m });
        }
        _selected_lines.append((ALine { _lines.last().endTime, eTime, _lines.last().endY, eY, 1 }));
    }

    _selectedStartIndex = startIndex;
    _selectedEndIndex = endIndex;

    if (startIndex >= 0 && startIndex < _lines.size())
        _selectedStartIndex = findRangeIndex(time2Pixel(_lines[startIndex].startTime), getYPixel(_lines[startIndex].startY), time2Pixel(_lines[startIndex].endTime), getYPixel(_lines[startIndex].endY));

    if (endIndex >= 0 && endIndex < _lines.size())
        _selectedEndIndex = findRangeIndex(time2Pixel(_lines[endIndex].startTime), getYPixel(_lines[endIndex].startY), time2Pixel(_lines[endIndex].endTime), getYPixel(_lines[endIndex].endY));

    _selected_point_index.clear();
    for (int i = startIndex + 1; i <= endIndex; i++) {
        _init_selected_point_move = true;

        _selected_point_index.append(i);
    }

    for (int i = 1; i < _selected_lines.size() - 1; i++) {

        _selected_point2.append(APoint { _selected_lines[i].startTime, _selected_lines[i].startY });
    }

    _selected_point2.append(APoint { _selected_lines.last().startTime, _selected_lines.last().startY });
}

QVector<Automation::ALine> Automation::createSelectedLines(double startMarker, double endMarker)
{
    if (startMarker > endMarker) {
        auto temp = endMarker;
        endMarker = startMarker;
        startMarker = temp;

        Q_EMIT sigStartMarkerChanged(startMarker);
        Q_EMIT sigEndMarkerChanged(endMarker);
    }
    QVector<Automation::ALine> selected_lines;
    if (_ranges.size() <= 1)
        return selected_lines;

    if (qAbs(startMarker - endMarker) <= 0.0) {
        return selected_lines;
    }

    if (_areaInfo->selectionArea()->isClipSelectedAtFirst == true || _areaInfo->selectionArea()->isClipSelectedAtLast == true) {
        return selected_lines;
    }

    float startPix = time2Pixel(startMarker);
    float endPix = time2Pixel(endMarker);

    int sIndex = findRangeIndex(startPix);
    int eIndex = findRangeIndex(endPix);
    float sY = MathHelper1::yCalculatorFirst(_ranges[sIndex].p1(), _ranges[sIndex].p2(), startPix);
    float eY = MathHelper1::yCalculatorLast(_ranges[eIndex].p1(), _ranges[eIndex].p2(), endPix);

    sY = (sY - _windowRect.top()) / (double)(_windowRect.bottom() - _windowRect.top());
    eY = (eY - _windowRect.top()) / (double)(_windowRect.bottom() - _windowRect.top());

    double sTime = startMarker;
    double eTime = endMarker;

    int startIndex = -1;
    int endIndex = _lines.count();
    for (int i = 0; i < _lines.count(); i++) {

        double start_time = _lines[i].startTime;
        double end_time = _lines[i].endTime;

        if (start_time < sTime && end_time > sTime) {
            startIndex = i;
            break;
        }
        if (qAbs(start_time - sTime) <= 0.001) {
            startIndex = i;
            break;
        }
        if (qAbs(start_time - sTime) <= 0.001 && qAbs(end_time - sTime) <= 0.001) {
            startIndex = i;
            break;
        }
    }
    for (int i = 0; i < _lines.count(); i++) {

        double start_time = _lines[i].startTime;
        double end_time = _lines[i].endTime;

        if (start_time < eTime && end_time > eTime) {
            endIndex = i;
        }

        if (qAbs(end_time - eTime) <= 0.001) {
            endIndex = i;
        }
        if (qAbs(start_time - eTime) <= 0.001 && qAbs(end_time - eTime) <= 0.001) {
            endIndex = i;
        }
    }
    if (sTime < _lines.first().startTime) {
        startIndex = -1;
    }
    if (sTime > _lines.last().endTime) {
        startIndex = _lines.count();
    }
    if (eTime < _lines.first().startTime) {
        endIndex = -1;
    }
    if (eTime > _lines.last().endTime) {
        endIndex = _lines.count();
    }

    if (qAbs(sY - 0) <= 0) {
        sY = _lines[startIndex].startY;
    }
    if (qAbs(eY - 0) <= 0) {
        eY = _lines[endIndex].endY;
    }

    if (startIndex > endIndex) {
        qDebug() << "BUG!2";
        return selected_lines;
    }

    if ((startIndex >= 0 && startIndex < _lines.size()) && (endIndex >= 0 && endIndex < _lines.size()) && startIndex != endIndex) {

        selected_lines.append((ALine { sTime, _lines[startIndex].endTime, sY, _lines[startIndex].endY, 1 }));
        for (int i = startIndex + 1; i <= endIndex - 1; i++) {
            selected_lines.append(ALine { _lines[i].startTime, _lines[i].endTime, _lines[i].startY, _lines[i].endY, _lines[i].curve_n, _lines[i].curve_m });
        }
        selected_lines.append((ALine { _lines[endIndex].startTime, eTime, _lines[endIndex].startY, eY, 1 }));
    }

    if ((startIndex >= 0 && startIndex < _lines.size()) && (endIndex >= 0 && endIndex < _lines.size()) && startIndex == endIndex) {

        selected_lines.clear();
        selected_lines.append((ALine { sTime, eTime, sY, eY, 1 }));
    }

    if (startIndex < 0 && (endIndex >= 0 && endIndex < _lines.size()) && startIndex != endIndex) {
        selected_lines.append((ALine { sTime, _lines.first().startTime, sY, _lines.first().startY, 1 }));
        for (int i = 0; i <= endIndex - 1; i++) {
            selected_lines.append(ALine { _lines[i].startTime, _lines[i].endTime, _lines[i].startY, _lines[i].endY, _lines[i].curve_n, _lines[i].curve_m });
        }
        selected_lines.append((ALine { _lines[endIndex].startTime, eTime, _lines[endIndex].startY, eY, 1 }));
    }

    if ((startIndex >= 0 && startIndex < _lines.size()) && endIndex >= _lines.size() && startIndex != endIndex) {

        selected_lines.append((ALine { sTime, _lines[startIndex].endTime, sY, _lines[startIndex].endY, 1 }));
        for (int i = startIndex + 1; i < _lines.size(); i++) {
            selected_lines.append(ALine { _lines[i].startTime, _lines[i].endTime, _lines[i].startY, _lines[i].endY, _lines[i].curve_n, _lines[i].curve_m });
        }
        selected_lines.append((ALine { _lines.last().endTime, eTime, _lines.last().endY, eY, 1 }));
    }

    if (startIndex < 0 && endIndex < 0) {
        selected_lines.append((ALine { sTime, eTime, sY, eY, 1 }));
    }

    if (startIndex >= _lines.size() && endIndex >= _lines.size()) {
        selected_lines.append((ALine { sTime, eTime, sY, eY, 1 }));
    }

    if (startIndex < 0 && endIndex >= _lines.size()) {
        selected_lines.append((ALine { sTime, _lines.first().startTime, sY, _lines.first().startY, 1 }));
        for (int i = 0; i < _lines.size(); i++) {
            selected_lines.append(ALine { _lines[i].startTime, _lines[i].endTime, _lines[i].startY, _lines[i].endY, _lines[i].curve_n, _lines[i].curve_m });
        }
        selected_lines.append((ALine { _lines.last().endTime, eTime, _lines.last().endY, eY, 1 }));
    }

    return selected_lines;
}

int Automation::getMaxYinSelectedPoints()
{

    if (_selectedLines && _selectedLines->size() > 0) {
        auto [minY, maxY] = _selectedLines->getMinMaxY();

        return getYPixel(maxY);
    } else if (_hoveredLines.size() > 0) {
        auto [minY, maxY] = _hoveredLines.getMinMaxY();
        return getYPixel(maxY);
    } else {
        if (m_selected_line && m_selected_line->size() > 0) {
            auto [minY, maxY] = m_selected_line->getMinMaxY();

            qDebug() << "MMM" << minY << maxY << getYPixel(maxY);
            return getYPixel(maxY);
        }
    }
    return _windowRect.top();
}

int Automation::getMinYinSelectedPoints()
{

    if (_selectedLines && _selectedLines->size() > 0) {
        auto [minY, maxY] = _selectedLines->getMinMaxY();
        return getYPixel(minY);
    } else if (_hoveredLines.size() > 0) {
        auto [minY, maxY] = _hoveredLines.getMinMaxY();
        return getYPixel(minY);
    } else {
        if (m_selected_line && m_selected_line->size() > 0) {
            auto [minY, maxY] = m_selected_line->getMinMaxY();
            return getYPixel(minY);
        }
    }

    return _windowRect.bottom();
}

bool Automation::isSelectedRange(int rangeIndex)
{

    auto ranges = selectedLines2Ranegs();
    auto range2 = getRanges(true);
    QLineF line = range2[rangeIndex];

    for (int i = 0; i < ranges.size(); i++) {
        if (line == ranges[i]) {
            return true;
        }
    }

    return false;
}

void Automation::streachSelectedLines(double _startTime, double duration, float down, float height, bool leftSide, bool rightSide, bool reverseUpDpwn, bool reverseLeftRight)
{

    if (_selected_lines.size() == 0)
        return;

    if (_init_selected_line_move) {

        _init_selected_line_move = false;
        _selected_lines_temp.clear();

        selectedTimeDuration = _selected_lines.last().endTime - _selected_lines.first().startTime;
        selectedTimeStart = _selected_lines.first().startTime;

        _start_index = find_first_selected_lines_index(_selected_lines.first().startTime);
        _end_index = find_last_selected_lines_index(_selected_lines.last().endTime);

        if (_start_index >= 0 && _start_index < _lines.size()) {
            if (qAbs(_selected_lines.first().startTime - _lines[_start_index].startTime) < 0.0001) {
                _start_index = _start_index - 1;
            }
            //            if (qAbs(_lines[_start_index].endTime - _lines[_start_index].startTime) < 0.0001) {
            //                _start_index = _start_index + 1;
            //            }
        }

        if (_end_index >= 0 && _end_index < _lines.size()) {
            if (qAbs(_selected_lines.last().endTime - _lines[_end_index].endTime) < 0.0001) {
                _end_index = _end_index + 1;
            }
            //            if (qAbs(_lines[_end_index].endTime - _lines[_end_index].startTime) < 0.0001) {
            //                _end_index = _end_index - 1;
            //            }
        }

        qDebug() << "START_POINT" << _selected_lines.size() << _start_index;
        _start_point_y = find_first_selected_lines_Y(_selected_lines, _start_index);
        _end_point_y = find_last_selected_lines_Y(_selected_lines, _end_index);

        float p_s, q_s, r_s, s_s, new_y_s;
        float p_l, q_l, r_l, s_l, new_y_l;
        bool result_s = findNewRoot(_start_index, _selected_lines.first().startTime, p_s, q_s, r_s, s_s, new_y_s);

        if (result_s) {
            _start_line = ALine { _selected_lines.first().startTime, _selected_lines.first().endTime, new_y_s, _lines[_start_index].endY, r_s, s_s };
        } else {

            _start_line = ALine { _selected_lines.first().startTime, _selected_lines.first().endTime, _start_point_y, _selected_lines.first().endY,
                _selected_lines.first().curve_n, _selected_lines.first().curve_m };
        }

        bool result_l = findNewRoot(_end_index, _selected_lines.last().endTime, p_l, q_l, r_l, s_l, new_y_l);

        if (result_l) {
            _end_line = ALine { _selected_lines.last().startTime, _selected_lines.last().endTime, _selected_lines.last().startY, new_y_l, p_l, q_l };
        } else {
            _end_line = ALine { _selected_lines.last().startTime, _selected_lines.last().endTime, _selected_lines.last().startY, _end_point_y,
                _selected_lines.last().curve_n, _selected_lines.last().curve_m };
        }

        _first_list.clear();
        _last_list.clear();

        for (int i = 0; i <= _start_index; i++) {
            if (_start_index >= 0 && _start_index < _lines.size()) {
                _first_list.append(_lines[i]);
            }
        }

        for (int i = _end_index; i < _lines.size(); i++) {

            if (_end_index >= 0 && _end_index < _lines.size()) {
                _last_list.append(_lines[i]);
            }
        }

        if (_lines.size() > 0) {
            if (_start_index >= 0 && _start_index < _lines.size() - 1) {

                if (result_s) {
                    _first_line = ALine { _lines[_start_index].startTime, _selected_lines.first().startTime, _lines[_start_index].startY, new_y_s, p_s, q_s };

                } else {
                    _first_line = ALine { _lines[_start_index].startTime, _selected_lines.first().startTime, _lines[_start_index].startY, _start_point_y };
                }

            } else if (_start_index >= _lines.size() - 1) {
                _first_line = ALine { _lines[_lines.size() - 1].startTime, _lines[_lines.size() - 1].endTime, _lines[_lines.size() - 1].startY, _lines[_lines.size() - 1].endY };
            } else if (_start_index < 0) {
                _first_line = ALine { _selected_lines.first().startTime, _selected_lines.first().startTime, _selected_lines.first().startY, _selected_lines.first().startY };
            }
        } else {
            _first_line = _selected_lines.first();
        }

        if (_lines.size() > 0) {
            if (_end_index >= 0 && _end_index < _lines.size()) {

                if (result_l) {
                    _last_line = ALine { _selected_lines.last().endTime, _lines[_end_index].endTime, new_y_l, _lines[_end_index].endY, r_l, s_l };
                } else {
                    _last_line = ALine { _selected_lines.last().endTime, _lines[_end_index].endTime, _end_point_y, _lines[_end_index].endY };
                }
            } else if (_end_index < 0) {
                _last_line = ALine { _lines[0].startTime, _lines[0].endTime, _lines[0].startY, _lines[0].endTime };
            } else if (_end_index >= _lines.size()) {
                _last_line = ALine {
                    _selected_lines.last().endTime,
                    _selected_lines.last().endTime,
                    _selected_lines.last().endY,
                    _selected_lines.last().endY
                };
            }
        } else {
            _last_line = _selected_lines.last();
        }

        for (int i = 0; i < _selected_lines.size(); i++) {

            _selected_lines[i].rStartTime = (_selected_lines[i].startTime - selectedTimeStart) / (double)(selectedTimeDuration);
            _selected_lines[i].rEndTime = (_selected_lines[i].endTime - selectedTimeStart) / (double)(selectedTimeDuration);
        }

        for (int i = 0; i < _selected_lines.size(); i++) {
            _selected_lines[i].baseEndY = _selected_lines[i].endY;
            _selected_lines[i].baseStartY = _selected_lines[i].startY;
            qDebug() << "BASE LASTRATE" << i << _selected_lines[i].baseEndY << _selected_lines[i].baseStartY;
        }

        for (int i = 0; i < _lines.size(); i++) {
            _selected_lines_temp.append(_lines[i]);
        }

        createSnapLine(true);

    } else {

        for (int i = 0; i < _selected_lines.size(); i++) {

            _selected_lines[i].startTime = _selected_lines[i].rStartTime * duration + _areaInfo->startTime();
            _selected_lines[i].endTime = _selected_lines[i].rEndTime * duration + _areaInfo->startTime();
        }

        bool moveUp = true;
        bool moveDown = true;
        for (int i = 0; i < _selected_lines.size(); i++) {

            if (_selected_lines[i].startY > 0 || _selected_lines[i].endY > 0) {
                moveUp = false;
            }
        }
        for (int i = 0; i < _selected_lines.size(); i++) {

            if (_selected_lines[i].startY < 1 || _selected_lines[i].endY < 1) {
                moveDown = false;
            }
        }

        if (moveUp) {
            setLimitUpStreach(true);
        } else {
            setLimitUpStreach(false);
        }

        if (moveDown) {
            setLimitDownStreach(true);
        } else {
            setLimitDownStreach(false);
        }

        for (int i = 0; i < _selected_lines.size(); i++) {

            double baseStart = _selected_lines[i].baseStartY;
            double baseEnd = _selected_lines[i].baseEndY;
            if (reverseUpDpwn) {
                baseStart = 1 - baseStart;
                baseEnd = 1 - baseEnd;
            }
            _selected_lines[i].startY = baseStart * height + down;
            _selected_lines[i].endY = baseEnd * height + down;

            qDebug() << "LASTRATE" << i << baseStart << baseEnd << _selected_lines[i].startY << _selected_lines[i].endY;
        }

        int start_in_first_list = find_first_first_list_index(_selected_lines.first().startTime);
        int end_in_last_list = find_last_last_list_index(_selected_lines.last().endTime);

        if (start_in_first_list >= 0 && start_in_first_list < _first_list.size()) {
            if (qAbs(_selected_lines.first().startTime - _first_list[start_in_first_list].startTime) < 0.0001) {
                start_in_first_list = start_in_first_list - 1;
            }
        }

        if (end_in_last_list >= 0 && end_in_last_list < _last_list.size()) {

            if (qAbs(_selected_lines.last().endTime - _last_list[end_in_last_list].endTime) < 0.0001) {
                end_in_last_list = end_in_last_list + 1;
            }
        }

        _selected_lines_temp.clear();
        _selected_point2.clear();

        int start_in_last_index = find_last_last_list_index(_selected_lines.first().startTime);
        int end_in_first_index = find_first_first_list_index(_selected_lines.last().endTime);

        float start_in_first_y = find_first_first_lines_Y(start_in_first_list);
        float end_in_last_y = find_last_last_lines_Y(end_in_last_list);

        float end_in_first_y = find_lines_Y(_first_list, _selected_lines.last().endTime, end_in_first_index);
        float start_in_last_y = find_lines_Y(_last_list, _selected_lines.first().startTime, start_in_last_index);

        ALine first_point_in_first_list;
        ALine last_point_in_last_list;

        ALine first_point_in_last_list;
        ALine last_point_in_first_list;
        qDebug() << "PRE" << end_in_first_index << start_in_first_list << start_in_last_index << end_in_last_list << _first_list.size() << _last_list.size();
        if (_first_list.size() > 0) {
            if (start_in_first_list >= 0 && start_in_first_list < _first_list.size()) {

                int index = find_first_selected_lines_index(_selected_lines.first().startTime);

                float p, q, r, s, new_y;
                bool result = findNewRoot(index, _selected_lines.first().startTime, p, q, r, s, new_y);

                if (result) {
                    first_point_in_first_list = ALine { _first_list[start_in_first_list].startTime, _selected_lines.first().startTime,
                        _first_list[start_in_first_list].startY, new_y, p, q };

                } else {
                    first_point_in_first_list = ALine { _first_list[start_in_first_list].startTime, _selected_lines.first().startTime,
                        _first_list[start_in_first_list].startY, start_in_first_y, _first_list[start_in_first_list].curve_n, _first_list[start_in_first_list].curve_m };
                }

            } else if (start_in_first_list >= _first_list.size()) {

                qDebug() << "BBB12";
                first_point_in_first_list = _first_line;

            } else if (start_in_first_list < 0) {
                qDebug() << "BBB13";
                first_point_in_first_list = ALine { _selected_lines.last().endTime, _lines.first().startTime,
                    _lines.first().startY, _lines.first().startY };
            }
        } else {
            qDebug() << "BBB15";
            first_point_in_first_list = ALine { _selected_lines.last().endTime, _lines.first().startTime,
                _lines.first().startY, _lines.first().startY };
        }

        if (_first_list.size() > 0) {

            if (end_in_first_index >= 0 && end_in_first_index <= _first_list.size() - 1) {

                int index = find_last_selected_lines_index(_selected_lines.last().endTime);

                float p, q, r, s, new_y;
                bool result = findNewRoot(index, _selected_lines.last().endTime, p, q, r, s, new_y);

                APoint sPoint = APoint { _first_list[end_in_first_index].endTime, _first_list[end_in_first_index].endY,
                    _first_list[end_in_first_index].curve_n, _first_list[end_in_first_index].curve_m };

                if (end_in_first_index == _first_list.size() - 1) {
                    sPoint = APoint { _first_line.endTime, _first_line.endY,
                        _first_line.curve_n, _first_line.curve_m };
                }

                if (result) {
                    last_point_in_first_list = ALine { _selected_lines.last().endTime, sPoint.time, new_y, sPoint.y, r, s };

                } else {
                    last_point_in_first_list = ALine { _selected_lines.last().endTime, sPoint.time, end_in_first_y, sPoint.y,
                        sPoint.curve_n, sPoint.curve_m };
                }

                qDebug() << "CCCC1" << end_in_first_index << _selected_lines.last().endTime << _first_list[end_in_first_index].endTime << end_in_first_y << _first_list[end_in_first_index].endY;
            } else if (end_in_first_index >= _first_list.size()) {
                qDebug() << "CCCC2";
                last_point_in_first_list = _first_line;

            } else if (end_in_first_index < 0) {
                qDebug() << "CCCC3";
                last_point_in_first_list = ALine { _selected_lines.last().endTime, _lines.first().startTime,
                    _lines.first().startY, _lines.first().startY };

            } else if (end_in_first_index == _first_list.size() - 1) {
                qDebug() << "CCCC4" << end_in_first_index;
                last_point_in_first_list = ALine { _selected_lines.last().endTime, _first_line.endTime,
                    end_in_first_y, _first_line.endY };
            }
        } else {
            qDebug() << "CCCC4";

            last_point_in_first_list = ALine { _selected_lines.last().endTime, _lines.first().startTime,
                _lines.first().startY, _lines.first().startY };
        }

        if (_last_list.size() > 0) {
            if (end_in_last_list >= 0 && end_in_last_list < _last_list.size()) {

                int index = find_last_selected_lines_index(_selected_lines.last().endTime);

                float p, q, r, s, new_y;
                bool result = findNewRoot(index, _selected_lines.last().endTime, p, q, r, s, new_y);

                qDebug() << "AAAA1" << result;
                if (result) {
                    last_point_in_last_list = ALine { _selected_lines.last().endTime, _last_list[end_in_last_list].endTime, new_y, _last_list[end_in_last_list].endY, r, s };

                } else {
                    last_point_in_last_list = ALine { _selected_lines.last().endTime, _last_list[end_in_last_list].endTime, end_in_last_y,
                        _last_list[end_in_last_list].endY, _last_list[end_in_last_list].curve_n, _last_list[end_in_last_list].curve_m };
                }

            } else if (end_in_last_list < 0) {

                qDebug() << "AAAA12";
                last_point_in_last_list = _last_line;

            } else if (end_in_last_list >= _last_list.size()) {

                qDebug() << "AAAA13";
                last_point_in_last_list = ALine { _lines.last().endTime, _selected_lines.first().startTime,
                    _lines.last().endY, _lines.last().endY };
            }
        } else {
            qDebug() << "AAAA14";
            last_point_in_last_list = ALine { _lines.last().endTime, _selected_lines.first().startTime,
                _lines.last().endY, _lines.last().endY };
        }

        if (_last_list.size() > 0) {
            if (start_in_last_index >= 0 && start_in_last_index < _last_list.size()) {

                qDebug() << "DDD1" << _selected_lines.size();
                int index = find_first_selected_lines_index(_selected_lines.first().startTime);

                float p, q, r, s, new_y;
                bool result = findNewRoot(index, _selected_lines.first().startTime, p, q, r, s, new_y);

                APoint sPoint = APoint { _last_list[start_in_last_index].startTime, _last_list[start_in_last_index].startY,
                    _last_list[start_in_last_index].curve_n, _last_list[start_in_last_index].curve_m };

                if (start_in_last_index == 0) {
                    sPoint = APoint { _last_line.startTime, _last_line.startY,
                        _last_line.curve_n, _last_line.curve_m };
                }

                if (result) {
                    first_point_in_last_list = ALine { sPoint.time, _selected_lines.first().startTime,
                        sPoint.y, new_y, p, q };

                } else {
                    first_point_in_last_list = ALine { sPoint.time, _selected_lines.first().startTime,
                        sPoint.y, start_in_last_y, sPoint.curve_n, sPoint.curve_m };
                }
            } else if (start_in_last_index < 0) {
                qDebug() << "DDD12";
                first_point_in_last_list = ALine { _selected_lines.first().startTime, _lines.first().startTime,
                    _lines.first().startY, _lines.first().startY };

            } else if (start_in_last_index >= _last_list.size()) {
                qDebug() << "DDD13";
                first_point_in_last_list = ALine { _lines.last().endTime, _selected_lines.first().startTime,
                    _lines.last().endY, _lines.last().endY };
            }

        } else {
            qDebug() << "DDD5";
            first_point_in_last_list = ALine { _last_line.startTime, _selected_lines.first().startTime,
                _last_line.startY, start_in_last_y };
        }

        for (int i = 0; i < start_in_first_list; i++) {
            if (i >= _first_list.size() - 1)
                continue;

            _selected_lines_temp.append(_first_list[i]);
        }

        qDebug() << "START" << _selected_lines_temp.size() << end_in_last_list << _last_list.size() << start_in_first_list << _first_list.size() << end_in_first_index << _last_list.size() << _selected_lines.first().startTime << _selected_lines.last().endTime << _start_line.startTime << _end_line.endTime;
        if (_selected_lines.first().startTime > _start_line.startTime && _selected_lines.first().startTime < _end_line.endTime) {
            qDebug() << "StageA0";
            if (isZero(_first_line.startY, _first_line.endY)) {
                if (isZero(_selected_lines.first().startY, _selected_lines.first().endY)) {
                    if (isZero(_selected_lines.first().endY, _first_line.startY)) {
                        qDebug() << "StageA1.1";
                        if (_first_list.size() > 0) {
                            qDebug() << "StageA1.2";
                            _selected_lines_temp.append(ALine { _first_line.startTime, _selected_lines.first().endTime,
                                _first_line.startY, _selected_lines.first().endY });
                        } else {
                            qDebug() << "StageA1.2.1";
                        }

                    } else {
                        qDebug() << "StageA1.3";

                        if (_first_list.size() > 0) {

                            _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                            _selected_lines_temp.append(ALine { _first_line.endTime, _selected_lines.first().startTime,
                                _first_line.endY, _selected_lines.first().startY });

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });

                        } else {
                            _selected_lines_temp.append(ALine { _first_line.startTime, _selected_lines.first().startTime,
                                _first_line.startY, _selected_lines.first().startY });

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                        }
                    }
                } else {

                    if (isZero(_selected_lines.first().startY, _first_line.startY)) {
                        qDebug() << "StageA1.5";
                        if (_first_list.size() > 0) {
                            qDebug() << "StageA1.6";
                            _selected_lines_temp.append(ALine { _first_line.startTime, _selected_lines.first().startTime,
                                _first_line.startY, _selected_lines.first().startY });

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                        } else {
                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                        }

                    } else {
                        qDebug() << "StageA1.7";
                        if (_first_list.size() > 0) {
                            qDebug() << "StageA1.8";
                            _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                _first_line.startY, _selected_lines.first().startY, _first_line.curve_n, _first_line.curve_m });

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                        } else {
                            _selected_lines_temp.append(ALine { _first_line.startTime, _selected_lines.first().startTime,
                                _first_line.startY, _selected_lines.first().startY });

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                        }
                    }
                }
            } else {
                qDebug() << "StageA0.1";
                if (isZero(_selected_lines.first().startY, _selected_lines.first().endY)) {
                    qDebug() << "StageA0.2";

                    if (isZero(_selected_lines.first().startY, _first_line.endY)) {
                        qDebug() << "StageA0.2.1";
                        _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                            _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                        _selected_lines_temp.append(ALine { _first_line.endTime, _selected_lines.first().endTime, _first_line.endY,
                            _selected_lines.first().endY });
                    } else {

                        qDebug() << "StageA0.2.2";
                        _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                            _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                        _selected_lines_temp.append(ALine { _first_line.endTime, _selected_lines.first().startTime, _first_line.endY,
                            _selected_lines.first().startY });

                        _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime, _selected_lines.first().startY,
                            _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                    }

                } else {
                    qDebug() << "StageA1";

                    if (_first_list.size() > 0) {
                        _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                            _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                        _selected_lines_temp.append(ALine { _first_line.endTime, _selected_lines.first().startTime,
                            _first_line.endY, _selected_lines.first().startY });
                    }

                    _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime, _selected_lines.first().startY,
                        _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                }
            }
        } else if (isZero(_selected_lines.first().startTime, _start_line.startTime)) {
            qDebug() << "StageC0";

            if (isZero(_selected_lines.first().startTime, _selected_lines.last().endTime)) {

                qDebug() << "StageC0.1";
                if (_first_list.size() > 0) {
                    _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                        _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });
                }

            } else {
                if (isZero(_selected_lines.first().startY, _start_line.startY)) {

                    qDebug() << "StageC1";

                    if (_first_list.size() == 0) {

                    } else {
                        if (isZero(_selected_lines.first().startY, _selected_lines.first().endY)
                            && isZero(first_point_in_first_list.startY, first_point_in_first_list.endY)) {

                            qDebug() << "StageC2";
                            _selected_lines_temp.append(ALine { first_point_in_first_list.startTime, _selected_lines.first().endTime,
                                first_point_in_first_list.startY, _selected_lines.first().endY });
                        } else {
                            qDebug() << "StageC3";

                            _selected_lines_temp.append(ALine { first_point_in_first_list.startTime, _selected_lines.first().startTime,
                                first_point_in_first_list.startY, _selected_lines.first().startY, first_point_in_first_list.curve_n, first_point_in_first_list.curve_m });

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                        }
                    }

                } else {

                    qDebug() << "StageC4";

                    if (_first_list.size() > 0) {
                        _selected_lines_temp.append(ALine { first_point_in_first_list.startTime, first_point_in_first_list.endTime,
                            first_point_in_first_list.startY, first_point_in_first_list.endY, first_point_in_first_list.curve_n, first_point_in_first_list.curve_m });
                    }

                    _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime, first_point_in_first_list.endY, _selected_lines.first().startY });

                    _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime, _selected_lines.first().startY,
                        _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                }
            }

        } else if (_selected_lines.first().startTime < _start_line.startTime) {
            qDebug() << "StageA3";

            if (isZero(_selected_lines.last().startTime, _selected_lines.last().endTime)) {
                qDebug() << "StageA11";
                if (reverseLeftRight) {
                    qDebug() << "StageA12";

                    if (_first_list.size() > 0) {
                        _selected_lines_temp.append(ALine { first_point_in_first_list.startTime, _selected_lines.last().endTime,
                            first_point_in_first_list.startY, _selected_lines.last().endY });
                    }

                } else {
                    qDebug() << "StageA13";

                    if (_first_list.size() > 0) {
                        _selected_lines_temp.append(ALine { first_point_in_first_list.startTime, _selected_lines.first().startTime,
                            first_point_in_first_list.startY, _selected_lines.first().startY });
                    }
                }
            } else {
                if (isZero(first_point_in_first_list.startY, first_point_in_first_list.endY)) {
                    if (isZero(_selected_lines.first().startY, _selected_lines.first().endY)) {
                        if (isZero(_selected_lines.first().endY, first_point_in_first_list.startY)) {
                            qDebug() << "StageA3.1";
                            if (start_in_first_list >= 0) {
                                qDebug() << "StageA3.2";
                                _selected_lines_temp.append(ALine { first_point_in_first_list.startTime, _selected_lines.first().endTime,
                                    first_point_in_first_list.startY, _selected_lines.first().endY });
                            } else {
                                qDebug() << "StageA3.2.1";
                            }

                        } else {
                            qDebug() << "StageA3.3";
                            if (start_in_first_list >= 0) {
                                qDebug() << "StageA3.4";
                                _selected_lines_temp.append(ALine { first_point_in_first_list.startTime, _selected_lines.first().startTime,
                                    first_point_in_first_list.startY, first_point_in_first_list.startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                    first_point_in_first_list.startY, _selected_lines.first().startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                    _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            } else {
                                qDebug() << "StageA3.4.1";
                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                    first_point_in_first_list.startY, _selected_lines.first().startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                    _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            }
                        }
                    } else {

                        if (isZero(_selected_lines.first().startY, first_point_in_first_list.endY)) {
                            qDebug() << "StageA3.5";
                            if (start_in_first_list >= 0) {
                                qDebug() << "StageA3.6";
                                _selected_lines_temp.append(ALine { first_point_in_first_list.startTime, _selected_lines.first().startTime,
                                    first_point_in_first_list.startY, _selected_lines.first().startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                    _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            } else {
                                qDebug() << "StageA3.6.1";

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                    _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            }

                        } else {
                            qDebug() << "StageA3.7";
                            if (start_in_first_list >= 0) {
                                qDebug() << "StageA3.8";
                                _selected_lines_temp.append(ALine { first_point_in_first_list.startTime, _selected_lines.first().startTime,
                                    first_point_in_first_list.startY, first_point_in_first_list.startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                    first_point_in_first_list.startY, _selected_lines.first().startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                    _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            } else {
                                qDebug() << "StageA3.8.1";
                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                    first_point_in_first_list.startY, _selected_lines.first().startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                    _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            }
                        }
                    }
                } else {
                    qDebug() << "StageA10";
                    if (isZero(_selected_lines.first().startY, _selected_lines.first().endY)) {

                        qDebug() << "StageA11";
                        if (start_in_first_list >= 0) {

                            qDebug() << "StageA11.1";
                            if (isZero(_selected_lines.first().startY, first_point_in_first_list.endY)) {
                                _selected_lines_temp.append(ALine { first_point_in_first_list.startTime, first_point_in_first_list.endTime,
                                    first_point_in_first_list.startY, first_point_in_first_list.endY, first_point_in_first_list.curve_n, first_point_in_first_list.curve_m });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime, _selected_lines.first().startY,
                                    _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            } else {
                                qDebug() << "StageA11.2";
                                _selected_lines_temp.append(ALine { first_point_in_first_list.startTime, first_point_in_first_list.endTime,
                                    first_point_in_first_list.startY, first_point_in_first_list.endY, first_point_in_first_list.curve_n, first_point_in_first_list.curve_m });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                    first_point_in_first_list.endY, _selected_lines.first().startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime, _selected_lines.first().startY,
                                    _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            }
                        } else {
                            qDebug() << "StageA11.3";
                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                first_point_in_first_list.startY, _selected_lines.first().startY });

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime, _selected_lines.first().startY,
                                _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                        }
                    } else {
                        qDebug() << "StageA9";

                        if (start_in_first_list >= 0) {

                            qDebug() << "StageA9.1";
                            if (isZero(_selected_lines.first().startY, first_point_in_first_list.endY)) {
                                _selected_lines_temp.append(ALine { first_point_in_first_list.startTime, first_point_in_first_list.endTime,
                                    first_point_in_first_list.startY, first_point_in_first_list.endY, first_point_in_first_list.curve_n, first_point_in_first_list.curve_m });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime, _selected_lines.first().startY,
                                    _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            } else {
                                qDebug() << "StageA9.2";
                                _selected_lines_temp.append(ALine { first_point_in_first_list.startTime, first_point_in_first_list.endTime,
                                    first_point_in_first_list.startY, first_point_in_first_list.endY, first_point_in_first_list.curve_n, first_point_in_first_list.curve_m });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                    first_point_in_first_list.endY, _selected_lines.first().startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime, _selected_lines.first().startY,
                                    _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            }
                        } else {
                            qDebug() << "StageA9.3";
                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                first_point_in_first_list.startY, _selected_lines.first().startY });

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime, _selected_lines.first().startY,
                                _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                        }
                    }
                }
            }
        } else if (isZero(_selected_lines.first().startTime, _end_line.endTime)) {
            qDebug() << "StageE";

            if (isZero(_selected_lines.first().startTime, _selected_lines.last().endTime)) {

                qDebug() << "StageE.1";
                if (reverseLeftRight) {
                    if (isZero(_selected_lines.last().endY, _first_line.endY) && isZero(_first_line.endY, _first_line.startY)) {
                        qDebug() << "StageE.2";

                        if (_first_list.size() > 0) {
                            _selected_lines_temp.append(ALine { _first_line.startTime, _selected_lines.last().endTime,
                                _first_line.startY, _selected_lines.last().endY });
                        }
                    } else {
                        qDebug() << "StageE.3";
                        _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                            _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                        _selected_lines_temp.append(ALine { _first_line.endTime, _selected_lines.last().endTime,
                            _first_line.endY, _selected_lines.last().endY });
                    }

                } else {
                    qDebug() << "StageE.4";
                    if (isZero(_selected_lines.first().startY, _first_line.endY) && isZero(_first_line.endY, _first_line.startY)) {
                        qDebug() << "StageE.5";

                        if (_first_list.size() > 0) {
                            _selected_lines_temp.append(ALine { _first_line.startTime, _selected_lines.first().startTime,
                                _first_line.startY, _selected_lines.first().startY });
                        }
                    } else {
                        qDebug() << "StageE.6";
                        _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                            _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                        _selected_lines_temp.append(ALine { _first_line.endTime, _selected_lines.first().startTime,
                            _first_line.endY, _selected_lines.first().startY });
                    }
                }
            } else {
                if (_first_list.size() == 0) {

                    qDebug() << "StageE.1";

                    if (isZero(_selected_lines.first().startY, _end_line.endY)) {
                        if (isZero(_selected_lines.first().startY, _first_line.startY)) {

                            qDebug() << "StageE.2";
                            if (isZero(_end_line.startY, _end_line.endY)) {
                                qDebug() << "StageE.3";
                                _selected_lines_temp.append(ALine { _first_line.startTime, _last_line.startTime,
                                    _first_line.startY, _last_line.startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                    _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            } else {
                                // Nothing
                            }
                        } else {
                            qDebug() << "StageE.41";
                            _selected_lines_temp.append(ALine { _first_line.startTime, _last_line.startTime,
                                _first_line.startY, _last_line.startY });

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                        }
                    } else {

                        _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                            _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                        _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                            _first_line.endY, _last_line.startY });

                        _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                            _last_line.startY, _selected_lines.first().startY });

                        _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                            _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                    }
                } else {

                    qDebug() << "StageE" << _start_line.startY << _start_line.endY << _end_line.endY << _end_line.startY;
                    if (isZero(_first_line.startY, _first_line.endY)) {

                        if (isZero(_selected_lines.first().startY, _selected_lines.first().endY)) {
                            if (isZero(_selected_lines.first().startY, _first_line.endY)) {
                                qDebug() << "StageE1";
                                _selected_lines_temp.append(ALine { _first_line.startTime, _selected_lines.first().endTime,
                                    _first_line.startY, _selected_lines.first().endY });
                            } else {
                                qDebug() << "StageE2";
                                _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                    _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                                _selected_lines_temp.append(ALine { _first_line.endTime, _selected_lines.first().startTime,
                                    _first_line.endY, _selected_lines.first().startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                    _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            }

                        } else {

                            qDebug() << "StageE3";

                            if (isZero(_selected_lines.first().startY, _first_line.endY)) {
                                qDebug() << "StageE4";
                                _selected_lines_temp.append(ALine { _first_line.startTime, _selected_lines.first().endTime,
                                    _first_line.startY, _selected_lines.first().endY });
                            } else {
                                qDebug() << "StageE5";
                                _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                    _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                                _selected_lines_temp.append(ALine { _first_line.endTime, _selected_lines.first().startTime,
                                    _first_line.endY, _selected_lines.first().startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                    _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            }
                        }

                    } else {

                        qDebug() << "StageE6";

                        if (isZero(_selected_lines.first().startY, _end_line.endY)) {
                            if (isZero(_selected_lines.first().startY, _selected_lines.first().endY)) {
                                if (isZero(_selected_lines.first().startY, _first_line.endY)) {
                                    qDebug() << "StageE7";
                                    _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                        _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                                    _selected_lines_temp.append(ALine { _first_line.endTime, _selected_lines.first().endTime,
                                        _first_line.endY, _selected_lines.first().endY });
                                } else {
                                    qDebug() << "StageE8";
                                    _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                        _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                                    _selected_lines_temp.append(ALine { _first_line.endTime, _selected_lines.first().startTime,
                                        _first_line.endY, _selected_lines.first().startY });

                                    _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                        _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                                }

                            } else {

                                qDebug() << "StageE9";

                                _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                    _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                                _selected_lines_temp.append(ALine { _first_line.endTime, _selected_lines.first().startTime,
                                    _first_line.endY, _selected_lines.first().startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                    _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            }
                        } else {

                            qDebug() << "StageE9.1";
                            _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                            _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                                _first_line.endY, _last_line.startY });

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                _last_line.startY, _selected_lines.first().startY });

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                        }
                    }
                }
            }

        } else if (_selected_lines.first().startTime > _end_line.endTime) {
            qDebug() << "StageG0";

            qDebug() << "StageG0.1" << _first_line.startY << _first_line.endY << first_point_in_last_list.startY << first_point_in_last_list.endY << start_in_last_index;

            if (start_in_last_index == 0 && _last_list.size() > 0) {

                if (isZero(_selected_lines.last().startTime, _selected_lines.last().endTime)) {
                    qDebug() << "StageG0.11";

                    _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                        _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                    _selected_lines_temp.append(ALine { _first_line.endTime, first_point_in_last_list.startTime,
                        _first_line.endY, first_point_in_last_list.startY });

                    if (reverseLeftRight) {
                        qDebug() << "StageG0.112";
                        _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, _selected_lines.last().endTime,
                            first_point_in_last_list.startY, _selected_lines.last().endY });
                    } else {
                        qDebug() << "StageG0.13";

                        _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, _selected_lines.first().startTime,
                            first_point_in_last_list.startY, _selected_lines.first().startY });
                    }
                } else {
                    if (isZero(_first_line.startY, _first_line.endY)) {
                        qDebug() << "StageG0.2";
                        if (isZero(_selected_lines.first().startY, _selected_lines.first().endY)) {
                            qDebug() << "StageG0.3";
                            if (isZero(_first_line.endY, first_point_in_last_list.startY)) {
                                qDebug() << "StageG0.4";
                                if (isZero(first_point_in_last_list.endY, first_point_in_last_list.startY)) {
                                    qDebug() << "StageG0.5";
                                    _selected_lines_temp.append(ALine { _first_line.startTime, _selected_lines.first().endTime,
                                        first_point_in_last_list.startY, _selected_lines.first().endY });
                                } else {
                                    qDebug() << "StageG0.6";
                                    _selected_lines_temp.append(ALine { _first_line.startTime, first_point_in_last_list.startTime,
                                        _first_line.startY, first_point_in_last_list.startY });

                                    _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, first_point_in_last_list.endTime,
                                        first_point_in_last_list.startY, first_point_in_last_list.endY, first_point_in_last_list.curve_n, first_point_in_last_list.curve_m });

                                    if (isZero(first_point_in_last_list.endY, first_point_in_last_list.startY)
                                        && isZero(_selected_lines.first().startY,
                                            first_point_in_last_list.startY)) {

                                    } else {
                                        _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                            first_point_in_last_list.endY, _selected_lines.first().startY });
                                    }

                                    _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                        _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                                }

                            } else {

                                qDebug() << "StageG0.7";
                                _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                    _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                                _selected_lines_temp.append(ALine { _first_line.endTime, first_point_in_last_list.startTime,
                                    _first_line.endY, first_point_in_last_list.startY });

                                _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, first_point_in_last_list.endTime,
                                    first_point_in_last_list.startY, first_point_in_last_list.endY, first_point_in_last_list.curve_n, first_point_in_last_list.curve_m });

                                if (isZero(first_point_in_last_list.endY, first_point_in_last_list.startY)
                                    && isZero(_selected_lines.first().startY,
                                        first_point_in_last_list.startY)) {

                                } else {
                                    _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                        first_point_in_last_list.endY, _selected_lines.first().startY });
                                }

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                    _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            }
                        } else {

                            qDebug() << "StageG0.3";
                            _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                            _selected_lines_temp.append(ALine { _first_line.endTime, first_point_in_last_list.startTime,
                                _first_line.endY, first_point_in_last_list.startY });

                            _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, first_point_in_last_list.endTime,
                                first_point_in_last_list.startY, first_point_in_last_list.endY, first_point_in_last_list.curve_n, first_point_in_last_list.curve_m });

                            if (isZero(first_point_in_last_list.endY, first_point_in_last_list.startY)
                                && isZero(_selected_lines.first().startY,
                                    first_point_in_last_list.startY)) {

                            } else {
                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                    first_point_in_last_list.endY, _selected_lines.first().startY });
                            }

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                        }
                    } else {
                        qDebug() << "StageG0.41";
                        _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                            _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                        _selected_lines_temp.append(ALine { _first_line.endTime, first_point_in_last_list.startTime,
                            _first_line.endY, first_point_in_last_list.startY });

                        _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, first_point_in_last_list.endTime,
                            first_point_in_last_list.startY, first_point_in_last_list.endY, first_point_in_last_list.curve_n, first_point_in_last_list.curve_m });

                        if (isZero(first_point_in_last_list.endY, first_point_in_last_list.startY)
                            && isZero(_selected_lines.first().startY,
                                first_point_in_last_list.startY)) {

                        } else {
                            qDebug() << "StageG0.42";
                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                first_point_in_last_list.endY, _selected_lines.first().startY });
                        }

                        _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                            _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                    }
                }
            } else {

                qDebug() << "StageG1";

                if (_last_list.size() > 0) {
                    if (isZero(_first_line.startY, _first_line.endY) && isZero(_last_list.first().startY, _last_list.first().endY)
                        && isZero(_first_line.startY, _last_list.first().endY)) {

                        qDebug() << "StageG2";

                        _selected_lines_temp.append(ALine { _first_line.startTime, _last_line.endTime,
                            _first_line.startY, _last_line.endY });

                    } else if (!isZero(_first_line.startY, _first_line.endY) && isZero(_last_list.first().startY, _last_list.first().endY)
                        && isZero(_first_line.endY, _last_list.first().startY)) {

                        qDebug() << "StageG3";
                        _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                            _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                        _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.endTime,
                            _first_line.endY, _last_line.endY });

                    } else {

                        qDebug() << "StageG4";
                        _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                            _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                        _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                            _first_line.endY, _last_line.startY });

                        _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                            _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                    }

                    qDebug() << "_selected_lines_temp1" << _selected_lines_temp.size();
                    for (int i = 1; i <= start_in_last_index - 1; i++) {
                        qDebug() << "StageG5" << i;
                        _selected_lines_temp.append(_last_list[i]);
                    }
                    qDebug() << "_selected_lines_temp2" << _selected_lines_temp.size();

                    if (isZero(_selected_lines.last().startTime, _selected_lines.last().endTime)) {
                        qDebug() << "StageG11";
                        if (reverseLeftRight) {
                            qDebug() << "StageG12";
                            _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, _selected_lines.last().endTime,
                                first_point_in_last_list.startY, _selected_lines.last().endY });
                        } else {
                            qDebug() << "StageG13";

                            _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, _selected_lines.first().startTime,
                                first_point_in_last_list.startY, _selected_lines.first().startY });
                        }
                    } else {
                        if (isZero(first_point_in_last_list.startY, first_point_in_last_list.endY) && isZero(first_point_in_last_list.startY, _selected_lines.first().startY)
                            && isZero(_selected_lines.first().startY, _selected_lines.first().endY)) {

                            qDebug() << "StageG6";
                            _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, _selected_lines.first().endTime,
                                first_point_in_last_list.startY, _selected_lines.first().endY });

                        } else if (isZero(first_point_in_last_list.startY, first_point_in_last_list.endY) && isZero(first_point_in_last_list.startY, _selected_lines.first().startY)
                            && !isZero(_selected_lines.first().startY, _selected_lines.first().endY)) {

                            qDebug() << "StageG7";
                            _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, first_point_in_last_list.endTime,
                                first_point_in_last_list.startY, first_point_in_last_list.endY, first_point_in_last_list.curve_n, first_point_in_last_list.curve_m });

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });

                        } else {

                            qDebug() << "StageG7.1" << first_point_in_last_list.startTime << first_point_in_last_list.endTime << _lines.last().endTime << _lines.first().startTime
                                     << _selected_lines.first().startTime << _selected_lines.first().endTime << _selected_lines.last().startTime << _selected_lines.last().startTime;

                            _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, first_point_in_last_list.endTime,
                                first_point_in_last_list.startY, first_point_in_last_list.endY, first_point_in_last_list.curve_n, first_point_in_last_list.curve_m });

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                first_point_in_last_list.endY, _selected_lines.first().startY });

                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                        }
                    }
                } else {
                    qDebug() << "StageG8";

                    if (isZero(_selected_lines.first().startTime, _selected_lines.last().endTime)) {

                        qDebug() << "StageG8.19";
                        if (reverseLeftRight) {

                            qDebug() << "StageG8.20";
                            _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                            _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                                _first_line.endY, _last_line.startY });

                            _selected_lines_temp.append(ALine { _last_line.startTime, _selected_lines.last().endTime,
                                _last_line.startY, _selected_lines.last().endY });

                        } else {

                            qDebug() << "StageG8.21";
                            _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                            _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                                _first_line.endY, _last_line.startY });

                            _selected_lines_temp.append(ALine { _last_line.startTime, _selected_lines.first().startTime,
                                _last_line.startY, _selected_lines.first().startY });
                        }

                    } else {

                        if (isZero(_first_line.startY, _first_line.endY)) {
                            if (isZero(first_point_in_last_list.startY, _selected_lines.first().startY)) {
                                if (isZero(_selected_lines.first().endY, _selected_lines.first().startY)) {
                                    if (isZero(_first_line.startY, first_point_in_last_list.startY)) {
                                        if (isZero(first_point_in_last_list.startY, first_point_in_last_list.endY)) {
                                            qDebug() << "StageG8.1";
                                            _selected_lines_temp.append(ALine { _first_line.startTime, _selected_lines.first().endTime,
                                                _first_line.startY, _selected_lines.first().endY });
                                        } else {
                                            qDebug() << "StageG8.2";
                                            _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                                _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                                            _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                                                _first_line.endY, _last_line.startY });

                                            _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, _selected_lines.first().startTime,
                                                first_point_in_last_list.startY, _selected_lines.first().startY });

                                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                                first_point_in_last_list.startY, _selected_lines.first().startY });

                                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                                _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                                        }
                                    } else {
                                        qDebug() << "StageG8.3";
                                        _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                            _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                                        _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                                            _first_line.endY, _last_line.startY });

                                        _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, _selected_lines.first().startTime,
                                            first_point_in_last_list.startY, _selected_lines.first().startY });

                                        _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                            first_point_in_last_list.startY, _selected_lines.first().startY });

                                        _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                            _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                                    }
                                } else {
                                    qDebug() << "StageG8.4";
                                    _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                        _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                                    _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                                        _first_line.endY, _last_line.startY });

                                    _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, _selected_lines.first().startTime,
                                        first_point_in_last_list.startY, _selected_lines.first().startY });

                                    _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                        first_point_in_last_list.startY, _selected_lines.first().startY });

                                    _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                        _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                                }
                            } else {

                                qDebug() << "StageG8.4.1";
                                _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                    _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                                _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                                    _first_line.endY, _last_line.startY });

                                _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, _selected_lines.first().startTime,
                                    first_point_in_last_list.startY, first_point_in_last_list.startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                    first_point_in_last_list.startY, _selected_lines.first().startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                    _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            }
                        } else {
                            qDebug() << "StageG8.5";

                            if (isZero(_selected_lines.first().endY, _selected_lines.first().startY)
                                && isZero(first_point_in_last_list.startY, _selected_lines.first().startY)) {

                                qDebug() << "StageG8.5.1";

                                _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                    _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                                _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                                    _first_line.endY, _last_line.startY });

                                _selected_lines_temp.append(ALine { _last_line.startTime, _selected_lines.first().endTime,
                                    _last_line.startY, _selected_lines.first().endY });

                            } else if (!isZero(_selected_lines.first().endY, _selected_lines.first().startY)
                                && isZero(first_point_in_last_list.startY, _selected_lines.first().startY)) {

                                qDebug() << "StageG8.5.2";

                                _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                    _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                                _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                                    _first_line.endY, _last_line.startY });

                                _selected_lines_temp.append(ALine { _last_line.startTime, _selected_lines.first().startTime,
                                    _last_line.startY, _selected_lines.first().startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                    _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });

                            } else {

                                qDebug() << "StageG8.5.3";
                                _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                                    _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                                _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                                    _first_line.endY, _last_line.startY });

                                _selected_lines_temp.append(ALine { first_point_in_last_list.startTime, _selected_lines.first().startTime,
                                    first_point_in_last_list.startY, first_point_in_last_list.startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().startTime,
                                    first_point_in_last_list.startY, _selected_lines.first().startY });

                                _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.first().endTime,
                                    _selected_lines.first().startY, _selected_lines.first().endY, _selected_lines.first().curve_n, _selected_lines.first().curve_m });
                            }
                        }
                    }
                }
            }
        }

        for (int i = 1; i < _selected_lines.size() - 1; i++) {

            if (qAbs(_selected_lines.first().startTime - _selected_lines.last().endTime) < 0.0001) {

                if (reverseLeftRight) {

                    if (isZero(_last_line.startTime, _selected_lines.last().endTime) && _last_list.size() > 0) {

                        _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.startTime,
                            _selected_lines.last().endY, _last_line.startY });

                    } else if (isZero(_first_line.endTime, _selected_lines.last().endTime) && _first_list.size() > 0) {

                        _selected_lines_temp.append(ALine {
                            _first_line.endTime,
                            _selected_lines.first().startTime,
                            _first_line.endY,
                            _selected_lines.first().startY,
                        });

                    } else {
                        _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _selected_lines.first().startTime,
                            _selected_lines.last().endY, _selected_lines.first().startY });
                    }
                } else {

                    if (isZero(_last_line.startTime, _selected_lines.first().startTime) && _last_list.size() > 0) {

                        _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _last_line.startTime,
                            _selected_lines.first().startY, _last_line.startY });

                    } else if (isZero(_first_line.endTime, _selected_lines.first().startTime) && _first_list.size() > 0) {

                        _selected_lines_temp.append(ALine { _first_line.endTime, _selected_lines.last().endTime,
                            _first_line.endY, _selected_lines.last().endY });

                    } else {

                        _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _selected_lines.last().endTime,
                            _selected_lines.first().startY, _selected_lines.last().endY });
                    }
                }
                break;
            }

            _selected_lines_temp.append(_selected_lines[i]);

            _selected_point2.append(APoint { _selected_lines[i].startTime, _selected_lines[i].startY });
        }

        qDebug() << "_selected_lines_temp0" << _selected_lines_temp.size() << _selected_lines.size() << _end_line.endTime << _start_line.startTime << _selected_lines.last().endTime;

        if (_selected_lines.last().endTime < _end_line.endTime && _selected_lines.last().endTime > _start_line.startTime) {

            qDebug() << "StageB2.1";
            if (isZero(_last_line.startY, _last_line.endY)) {
                if (isZero(_selected_lines.last().startY, _selected_lines.last().endY)) {
                    if (isZero(_selected_lines.last().endY, _last_line.endY)) {
                        qDebug() << "StageB1.1";
                        if (_last_list.size() > 0) {
                            qDebug() << "StageB1.2";
                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _last_line.endTime,
                                _selected_lines.last().startY, _last_line.endY });
                        } else {
                            qDebug() << "StageB1.2.1";
                        }

                    } else {
                        qDebug() << "StageB1.3";
                        if (_last_list.size() > 0) {
                            qDebug() << "StageB1.4";

                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, last_point_in_last_list.startTime,
                                _selected_lines.last().endY, last_point_in_last_list.startY });

                            _selected_lines_temp.append(ALine { last_point_in_last_list.startTime, last_point_in_last_list.endTime,
                                last_point_in_last_list.startY, last_point_in_last_list.endY, last_point_in_last_list.curve_n, last_point_in_last_list.curve_m });

                        } else {

                            qDebug() << "StageB2.3";
                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.endTime,
                                _selected_lines.last().endY, _last_line.endY });
                        }
                    }
                } else {

                    if (isZero(_selected_lines.last().startY, _last_line.endY)) {
                        qDebug() << "StageB1.5";
                        if (_last_list.size() > 0) {
                            qDebug() << "StageB1.6";

                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.startTime,
                                _selected_lines.last().endY, _last_line.startY });

                        } else {
                            qDebug() << "StageB1.6.1";
                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.startTime,
                                _selected_lines.last().endY, _last_line.startY });
                        }

                    } else {
                        qDebug() << "StageB1.7";
                        _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                            _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                        _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.endTime,
                            _selected_lines.last().endY, _last_line.endY });
                    }
                }
            } else {
                qDebug() << " StageB2.1.1 ";
                if (isZero(_selected_lines.last().startY, _selected_lines.last().endY)) {
                    if (isZero(_selected_lines.last().startY, _last_line.endY)) {

                        qDebug() << "StageB2.1.2.1";
                        _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _last_line.startTime, _selected_lines.last().startY,
                            _last_line.startY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                        _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                            _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                    } else {
                        qDebug() << "StageB2.1.2.2";

                        _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime, _selected_lines.last().startY,
                            _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                        _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.startTime, _selected_lines.last().endY,
                            _last_line.startY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                        _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                            _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                    }

                } else {
                    qDebug() << "StageB11";

                    if (_last_list.size() > 0) {

                        qDebug() << "StageB12";

                        _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime, _selected_lines.last().startY,
                            _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                        _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.startTime,
                            _selected_lines.last().endY, _last_line.startY });

                        _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                            _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                    } else {

                        qDebug() << "StageB13";
                        _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime, _selected_lines.last().startY,
                            _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });
                    }
                }
            }

        } else if (isZero(_selected_lines.last().endTime, _end_line.endTime)) {
            qDebug() << "StageD1";

            if (isZero(_selected_lines.first().startTime, _selected_lines.last().endTime)) {

                _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                    _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
            } else {
                if (isZero(_selected_lines.last().endY, _end_line.endY)) {

                    qDebug() << "StageB1";

                    if (_last_list.size() == 0) {

                    } else {
                        if (isZero(_selected_lines.last().endY, _selected_lines.last().startY)
                            && isZero(_last_line.startY, _last_line.endY)) {

                            qDebug() << "StageB5.1" << _selected_lines.size();
                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _last_line.endTime,
                                _selected_lines.last().startY, _last_line.endY });

                        } else {
                            qDebug() << "StageB5";

                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.endTime,
                                _selected_lines.last().endY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                        }
                    }

                } else {

                    qDebug() << "StageB2";

                    _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime, _selected_lines.last().startY,
                        _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                    _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _selected_lines.last().endTime, _selected_lines.last().endY,
                        _last_line.startY });

                    if (_last_list.size() > 0) {
                        _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                            _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                    }
                }
            }
        } else if (isZero(_selected_lines.last().endTime, _start_line.startTime)) {
            qDebug() << "StageI0";

            if (isZero(_selected_lines.first().startTime, _selected_lines.last().endTime)) {

                if (reverseLeftRight) {
                    if (isZero(_selected_lines.first().startY, _last_line.endY) && isZero(_last_line.endY, _last_line.startY)) {

                        if (_last_list.size() > 0)
                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _last_line.endTime,
                                _selected_lines.first().startY, _last_line.endY });
                    } else {
                        _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _last_line.startTime,
                            _selected_lines.first().startY, _last_line.startY });

                        _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                            _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                    }
                } else {
                    if (isZero(_selected_lines.last().endY, _last_line.endY) && isZero(_last_line.endY, _last_line.startY)) {

                        if (_last_list.size() > 0)
                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.endTime,
                                _selected_lines.last().endY, _last_line.endY });
                    } else {
                        _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.startTime,
                            _selected_lines.last().endY, _last_line.startY });

                        _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                            _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                    }
                }

            } else {
                if (_first_list.size() == 0) {

                    qDebug() << "StageI0.1";

                    if (isZero(_selected_lines.last().endY, _start_line.startY)) {

                        if (isZero(_selected_lines.last().endY, _selected_lines.last().startY)) {
                            if (isZero(_end_line.startY, _end_line.endY)) {
                                if (isZero(_selected_lines.last().startY, _end_line.endY)) {

                                    qDebug() << "StageI0.2";
                                    _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _last_line.endTime,
                                        _selected_lines.last().startY, _last_line.endY });

                                } else {
                                    qDebug() << "StageI0.3";
                                    _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                        _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                                    _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                                        _first_line.endY, _last_line.startY });

                                    _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                        _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                                }
                            } else {
                                if (isZero(_selected_lines.last().startY, _end_line.startY)) {

                                    qDebug() << "StageI0.4";
                                    _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _last_line.startTime,
                                        _selected_lines.last().startY, _last_line.startY });

                                    _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                        _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });

                                } else {

                                    qDebug() << "StageI0.5";
                                    _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                        _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                                    _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.startTime,
                                        _selected_lines.last().endY, _last_line.startY });

                                    _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                        _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                                }
                            }
                        } else {
                            if (isZero(_end_line.startY, _end_line.endY)) {
                                if (isZero(_selected_lines.last().endY, _end_line.endY)) {

                                    qDebug() << "StageI0.6";
                                    _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                        _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                                    _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _last_line.endTime,
                                        _selected_lines.last().startY, _last_line.endY });
                                } else {
                                    qDebug() << "StageI0.7";
                                    _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                        _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                                    _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                                        _first_line.endY, _last_line.startY });

                                    _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                        _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                                }
                            } else {

                                qDebug() << "StageI0.8";
                                _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                    _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                                _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                                    _first_line.endY, _last_line.startY });

                                _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                    _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                            }
                        }

                    } else {

                        _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                            _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                        _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _selected_lines.last().endTime,
                            _selected_lines.last().endY, _start_line.startY });

                        _selected_lines_temp.append(ALine { _start_line.startTime, _last_line.startTime,
                            _start_line.startY, _last_line.startY });

                        _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                            _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                    }

                } else {

                    qDebug() << "StageI0" << _start_line.startY << _start_line.endY << _end_line.endY << _end_line.startY;

                    if (isZero(_last_line.startY, _last_line.endY)) {

                        if (isZero(_selected_lines.last().startY, _selected_lines.last().endY)) {
                            if (isZero(_selected_lines.last().endY, _last_line.startY)) {
                                qDebug() << "StageI1";
                                _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _last_line.endTime,
                                    _selected_lines.last().startY, _last_line.endY });
                            } else {
                                qDebug() << "StageI2";

                                _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                    _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                                _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.startTime,
                                    _selected_lines.last().endY, _last_line.startY });

                                _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                    _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                            }

                        } else {

                            qDebug() << "StageI3";

                            if (isZero(_selected_lines.last().endY, _last_line.startY)) {
                                qDebug() << "StageI4";
                                _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _last_line.endTime,
                                    _selected_lines.last().startY, _last_line.endY });

                            } else {
                                qDebug() << "StageI5";
                                _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                    _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                                _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.startTime,
                                    _selected_lines.last().endY, _last_line.startY });

                                _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                    _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                            }
                        }

                    } else {

                        qDebug() << "StageI6";

                        if (isZero(_selected_lines.last().endY, _start_line.startY)) {
                            if (isZero(_selected_lines.last().startY, _selected_lines.last().endY)) {
                                if (isZero(_selected_lines.last().endY, _last_line.startY)) {

                                    qDebug() << "StageI7";
                                    _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _last_line.startTime,
                                        _selected_lines.last().startY, _last_line.startY });

                                    _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                        _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                                } else {
                                    qDebug() << "StageI8";

                                    _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                        _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                                    _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.startTime,
                                        _selected_lines.last().endY, _last_line.startY });

                                    _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                        _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                                }

                            } else {

                                qDebug() << "StageI9";

                                _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                    _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                                _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _last_line.startTime,
                                    _selected_lines.last().endY, _last_line.startY });

                                _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                    _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                            }

                        } else {
                            qDebug() << "StageI6.1";

                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _selected_lines.last().endTime,
                                _selected_lines.last().endY, _start_line.startY });

                            _selected_lines_temp.append(ALine { _start_line.startTime, _last_line.startTime,
                                _start_line.startY, _last_line.startY });

                            _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                        }
                    }
                }
            }
        } else if (_selected_lines.last().endTime > _end_line.endTime) {
            qDebug() << "StageF" << end_in_last_list << _last_list.size();

            if (isZero(_selected_lines.last().startTime, _selected_lines.last().endTime)) {
                qDebug() << "StageF11";
                if (reverseLeftRight) {
                    qDebug() << "StageF12";

                    if (_last_list.size() > 0)
                        _selected_lines_temp.append(ALine { _selected_lines.first().startTime, last_point_in_last_list.endTime,
                            _selected_lines.first().startY, last_point_in_last_list.endY });

                } else {
                    qDebug() << "StageF13";

                    if (_last_list.size() > 0)
                        _selected_lines_temp.append(ALine { _selected_lines.last().endTime, last_point_in_last_list.endTime,
                            _selected_lines.last().endY, last_point_in_last_list.endY });
                }
            } else {
                if (isZero(last_point_in_last_list.startY, last_point_in_last_list.endY)) {
                    if (isZero(_selected_lines.last().endY, _selected_lines.last().startY)) {
                        if (isZero(_selected_lines.last().endY, last_point_in_last_list.endY)) {
                            qDebug() << "StageF1";

                            if (end_in_last_list < _last_list.size()) {
                                _selected_lines_temp.append(ALine { _selected_lines.last().startTime, last_point_in_last_list.endTime,
                                    _selected_lines.last().startY, last_point_in_last_list.endY });
                            }
                        } else {
                            qDebug() << "StageF2";

                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _selected_lines.last().endTime,
                                _selected_lines.last().endY, last_point_in_last_list.startY });

                            if (end_in_last_list < _last_list.size()) {
                                _selected_lines_temp.append(ALine { last_point_in_last_list.startTime, last_point_in_last_list.endTime,
                                    last_point_in_last_list.startY, last_point_in_last_list.endY, last_point_in_last_list.curve_n, last_point_in_last_list.curve_m });
                            }
                        }
                    } else {
                        if (isZero(_selected_lines.last().endY, last_point_in_last_list.endY)) {
                            qDebug() << "StageF3";

                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                            if (end_in_last_list < _last_list.size()) {
                                _selected_lines_temp.append(ALine { last_point_in_last_list.startTime, last_point_in_last_list.endTime,
                                    last_point_in_last_list.endY, last_point_in_last_list.endY, last_point_in_last_list.curve_n, last_point_in_last_list.curve_m });
                            }
                        } else {
                            qDebug() << "StageF4";
                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _selected_lines.last().endTime,
                                _selected_lines.last().endY, last_point_in_last_list.startY });

                            if (end_in_last_list < _last_list.size()) {
                                _selected_lines_temp.append(ALine { last_point_in_last_list.startTime, last_point_in_last_list.endTime,
                                    last_point_in_last_list.startY, last_point_in_last_list.endY, last_point_in_last_list.curve_n, last_point_in_last_list.curve_m });
                            }
                        }
                    }
                } else {
                    if (isZero(_selected_lines.last().endY, last_point_in_last_list.startY)) {
                        qDebug() << "StageF5";
                        _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                            _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                        if (end_in_last_list < _last_list.size()) {
                            _selected_lines_temp.append(ALine { last_point_in_last_list.startTime, last_point_in_last_list.endTime,
                                last_point_in_last_list.endY, last_point_in_last_list.endY, last_point_in_last_list.curve_n, last_point_in_last_list.curve_m });
                        }
                    } else {
                        qDebug() << "StageF6";
                        _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                            _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                        _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _selected_lines.last().endTime,
                            _selected_lines.last().endY, last_point_in_last_list.startY });

                        if (end_in_last_list < _last_list.size()) {
                            _selected_lines_temp.append(ALine { last_point_in_last_list.startTime, last_point_in_last_list.endTime,
                                last_point_in_last_list.startY, last_point_in_last_list.endY, last_point_in_last_list.curve_n, last_point_in_last_list.curve_m });
                        }
                    }
                }
            }

        } else if (_selected_lines.last().endTime < _start_line.startTime) {

            qDebug() << "StageK0";

            qDebug() << "Stagek.1" << _first_line.startY << _first_line.endY << last_point_in_first_list.startY << last_point_in_first_list.endY << end_in_first_index << _first_list.size();

            if (end_in_first_index == _first_list.size() - 1 && _first_list.size() > 0) {

                if (isZero(_selected_lines.last().startTime, _selected_lines.last().endTime)) {
                    qDebug() << "StageK11.1";
                    if (reverseLeftRight) {
                        qDebug() << "StageK12.1";
                        _selected_lines_temp.append(ALine { _selected_lines.first().startTime, last_point_in_first_list.endTime,
                            _selected_lines.first().startY, last_point_in_first_list.endY });
                    } else {
                        qDebug() << "StageK13.1";

                        _selected_lines_temp.append(ALine { _selected_lines.last().endTime, last_point_in_first_list.endTime,
                            _selected_lines.last().endY, last_point_in_first_list.endY });
                    }

                    _selected_lines_temp.append(ALine { last_point_in_first_list.endTime, _last_line.startTime,
                        last_point_in_first_list.endY, _last_line.startY });

                    _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                        _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });

                } else {
                    if (isZero(_last_line.startY, _last_line.endY)) {
                        if (isZero(_selected_lines.last().endY, _selected_lines.last().startY)) {
                            if (isZero(_last_line.startY, last_point_in_first_list.endY)) {
                                if (isZero(last_point_in_first_list.startY, last_point_in_first_list.endY)) {
                                    qDebug() << "Stagek1.1";
                                    _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _last_line.endTime,
                                        _selected_lines.last().startY, _last_line.endY });
                                } else {

                                    qDebug() << "Stagek1.2";

                                    _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                        _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                                    if (isZero(last_point_in_first_list.endY, last_point_in_first_list.startY)
                                        && isZero(_selected_lines.last().endY,
                                            last_point_in_first_list.startY)) {

                                    } else {
                                        _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _selected_lines.last().endTime,
                                            _selected_lines.last().startY, last_point_in_first_list.startY });
                                    }

                                    _selected_lines_temp.append(ALine { last_point_in_first_list.startTime, last_point_in_first_list.endTime,
                                        last_point_in_first_list.startY, last_point_in_first_list.endY });

                                    _selected_lines_temp.append(ALine { last_point_in_first_list.endTime, _last_line.endTime,
                                        last_point_in_first_list.endY, _last_line.endY });
                                }
                            } else {

                                qDebug() << "Stagek1.3";

                                _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                    _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                                if (isZero(last_point_in_first_list.endY, last_point_in_first_list.startY)
                                    && isZero(_selected_lines.last().endY,
                                        last_point_in_first_list.startY)) {

                                } else {
                                    _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _selected_lines.last().endTime,
                                        _selected_lines.last().startY, last_point_in_first_list.startY });
                                }

                                _selected_lines_temp.append(ALine { last_point_in_first_list.startTime, last_point_in_first_list.endTime,
                                    last_point_in_first_list.startY, last_point_in_first_list.endY });

                                _selected_lines_temp.append(ALine { last_point_in_first_list.endTime, _last_line.startTime,
                                    last_point_in_first_list.endY, _last_line.startY });

                                _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                    _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                            }
                        } else {
                            qDebug() << "StageGK.3";

                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                            if (isZero(last_point_in_first_list.endY, last_point_in_first_list.startY)
                                && isZero(_selected_lines.last().endY,
                                    last_point_in_first_list.startY)) {

                                qDebug() << "StageGK.3.1";
                            } else {
                                _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _selected_lines.last().endTime,
                                    _selected_lines.last().endY, last_point_in_first_list.startY });
                            }

                            _selected_lines_temp.append(ALine { last_point_in_first_list.startTime, last_point_in_first_list.endTime,
                                last_point_in_first_list.startY, last_point_in_first_list.endY });

                            _selected_lines_temp.append(ALine { last_point_in_first_list.endTime, _last_line.startTime,
                                last_point_in_first_list.endY, _last_line.startY });

                            _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                        }
                    } else {
                        qDebug() << "StageK0.4";

                        _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                            _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                        if (isZero(last_point_in_first_list.endY, last_point_in_first_list.startY)
                            && isZero(_selected_lines.last().endY,
                                last_point_in_first_list.startY)) {

                            qDebug() << "StageK0.4.1";
                        } else {
                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _selected_lines.last().endTime,
                                _selected_lines.last().endY, last_point_in_first_list.startY });
                        }

                        _selected_lines_temp.append(ALine { last_point_in_first_list.startTime, last_point_in_first_list.endTime,
                            last_point_in_first_list.startY, last_point_in_first_list.endY, last_point_in_first_list.curve_n, last_point_in_first_list.curve_m });

                        _selected_lines_temp.append(ALine { last_point_in_first_list.endTime, _last_line.startTime,
                            last_point_in_first_list.endY, _last_line.startY });

                        _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                            _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                    }
                }
            } else {

                if (_first_list.size() > 0) {

                    if (isZero(_selected_lines.last().startTime, _selected_lines.last().endTime)) {
                        qDebug() << "StageK11";
                        if (reverseLeftRight) {
                            qDebug() << "StageK12";
                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, last_point_in_first_list.endTime,
                                _selected_lines.first().startY, last_point_in_first_list.endY });
                        } else {
                            qDebug() << "StageK13";

                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, last_point_in_first_list.endTime,
                                _selected_lines.last().endY, last_point_in_first_list.endY });
                        }
                    } else {
                        if (isZero(last_point_in_first_list.startY, last_point_in_first_list.endY) && isZero(last_point_in_first_list.startY, _selected_lines.last().startY)
                            && isZero(_selected_lines.last().startY, _selected_lines.last().endY)) {

                            qDebug() << "StageK6";
                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, last_point_in_first_list.endTime,
                                _selected_lines.last().startY, last_point_in_first_list.endY });

                        } else if (isZero(last_point_in_first_list.startY, last_point_in_first_list.endY) && isZero(last_point_in_first_list.startY, _selected_lines.last().endY)
                            && !isZero(_selected_lines.last().endY, _selected_lines.last().startY)) {

                            qDebug() << "StageK7";

                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                            _selected_lines_temp.append(ALine { last_point_in_first_list.startTime, last_point_in_first_list.endTime,
                                last_point_in_first_list.startY, last_point_in_first_list.endY });

                        } else {

                            qDebug() << "StageK8";

                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _selected_lines.last().endTime,
                                _selected_lines.last().endY, last_point_in_first_list.startY });

                            _selected_lines_temp.append(ALine { last_point_in_first_list.startTime, last_point_in_first_list.endTime,
                                last_point_in_first_list.startY, last_point_in_first_list.endY });
                        }
                    }

                    qDebug() << "_selected_lines_temp12" << _selected_lines_temp.size();
                    for (int i = end_in_first_index + 1; i < _first_list.size() - 1; i++) {
                        qDebug() << "StageK8.1" << i;
                        _selected_lines_temp.append(_first_list[i]);
                    }

                    qDebug() << "_selected_lines_temp13" << _selected_lines_temp.size();

                    if (isZero(_first_line.startY, _first_line.endY) && isZero(_last_line.startY, _last_line.endY)
                        && isZero(_first_line.startY, _last_list.first().endY)) {

                        qDebug() << "StageK8.2";

                        _selected_lines_temp.append(ALine { _first_line.startTime, _last_line.endTime,
                            _first_line.startY, _last_line.endY });

                    } else if (!isZero(_last_line.startY, _last_line.endY) && isZero(_first_line.startY, _first_line.endY)
                        && isZero(_first_line.endY, _last_list.first().startY)) {

                        qDebug() << "Stagk8.3";

                        _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                            _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                        _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.endTime,
                            _first_line.endY, _last_line.endY });

                    } else {

                        qDebug() << "Stagek8.4";
                        _selected_lines_temp.append(ALine { _first_line.startTime, _first_line.endTime,
                            _first_line.startY, _first_line.endY, _first_line.curve_n, _first_line.curve_m });

                        _selected_lines_temp.append(ALine { _first_line.endTime, _last_line.startTime,
                            _first_line.endY, _last_line.startY });

                        _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                            _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                    }

                } else {

                    if (isZero(_selected_lines.last().startTime, _selected_lines.last().endTime)) {
                        // To DO
                        qDebug() << "Stagek20";

                        if (reverseLeftRight) {
                            qDebug() << "Stagek21";
                            _selected_lines_temp.append(ALine { _selected_lines.first().startTime, _first_line.startTime,
                                _selected_lines.first().startY, _first_line.startY });

                            _selected_lines_temp.append(ALine { _first_line.startTime, _last_line.startTime,
                                _first_line.startY, _last_line.startY });
                            _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });

                        } else {
                            qDebug() << "Stagek22";
                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _first_line.startTime,
                                _selected_lines.last().endY, _first_line.startY });

                            _selected_lines_temp.append(ALine { _first_line.startTime, _last_line.startTime,
                                _first_line.startY, _last_line.startY });
                            _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                        }
                    } else {

                        qDebug() << "Stagek9";
                        if (isZero(_selected_lines.last().startY, _selected_lines.last().endY)) {
                            if (isZero(_first_line.startY, _selected_lines.last().endY)) {
                                if (isZero(_last_line.startY, _last_line.endY)) {
                                    if (isZero(_first_line.startY, _last_line.endY)) {
                                        qDebug() << "Stagek9.1";
                                        _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _last_line.endTime,
                                            _selected_lines.last().startY, _last_line.endY });
                                    } else {
                                        _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _first_line.startTime,
                                            _selected_lines.last().startY, _first_line.startY });

                                        _selected_lines_temp.append(ALine { _first_line.startTime, _last_line.startTime,
                                            _first_line.startY, _last_line.startY });

                                        _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                            _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                                    }
                                } else {
                                    qDebug() << "Stagek9.2";

                                    _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _first_line.startTime,
                                        _selected_lines.last().startY, _first_line.startY });

                                    _selected_lines_temp.append(ALine { _first_line.startTime, _last_line.startTime,
                                        _first_line.startY, _last_line.startY });

                                    _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                        _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                                }

                            } else {
                                qDebug() << "Stagek9.4";
                                _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                    _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                                _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _selected_lines.last().endTime,
                                    _selected_lines.last().endY, _first_line.startY });

                                _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _first_line.startTime,
                                    _first_line.startY, _first_line.startY });

                                _selected_lines_temp.append(ALine { _first_line.startTime, _last_line.startTime,
                                    _first_line.startY, _last_line.startY });

                                _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                    _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                            }
                        } else {
                            qDebug() << "Stagek9.5";
                            _selected_lines_temp.append(ALine { _selected_lines.last().startTime, _selected_lines.last().endTime,
                                _selected_lines.last().startY, _selected_lines.last().endY, _selected_lines.last().curve_n, _selected_lines.last().curve_m });

                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _selected_lines.last().endTime,
                                _selected_lines.last().endY, _first_line.startY });

                            _selected_lines_temp.append(ALine { _selected_lines.last().endTime, _first_line.startTime,
                                _first_line.startY, _first_line.startY });

                            _selected_lines_temp.append(ALine { _first_line.startTime, _last_line.startTime,
                                _first_line.startY, _last_line.startY });

                            _selected_lines_temp.append(ALine { _last_line.startTime, _last_line.endTime,
                                _last_line.startY, _last_line.endY, _last_line.curve_n, _last_line.curve_m });
                        }
                    }
                }
            }
        }

        qDebug() << "_selected_lines_temp1" << _selected_lines_temp.size() << _last_list.size();

        for (int i = end_in_last_list + 1; i < _last_list.size(); i++) {

            if (i > 0)
                _selected_lines_temp.append(_last_list[i]);
        }
    }

    qDebug() << "streachUpChecking";
    QVector<ALine> _selected_lines_temp_temp = streachUpChecking();
    _selected_lines_temp_temp = streachDownChecking(_selected_lines_temp_temp);
    _selected_lines_temp = _selected_lines_temp_temp;

    qDebug() << "_selected_lines_temp4" << _selected_lines_temp.size();
}
QVector<Automation::ALine> Automation::streachUpChecking()
{
    QVector<ALine> _selected_lines_temp_temp;

    double t1, t2;
    int index1, index2;
    double new_curve_n_1 = 1.0;
    double new_curve_m_1 = 1.0;
    double new_curve_n_2 = 1.0;
    double new_curve_m_2 = 1.0;
    for (int i = 0; i < _selected_lines_temp.size(); i++) {

        int start_y = getYPixel(_selected_lines_temp[i].startY);
        int end_y = getYPixel(_selected_lines_temp[i].endY);
        double curve_n = _selected_lines_temp[i].curve_n;
        double curve_m = _selected_lines_temp[i].curve_m;

        if (end_y < _windowRect.top() && start_y > _windowRect.top()) {
            auto t1_temp = getStartTime(0, _selected_lines_temp, i);

            if (curve_n != 1.0 || curve_n != 1.0) {
                QPointF a = QPointF(time2Pixel(_selected_lines_temp[i].endTime), getYPixel(_selected_lines_temp[i].endY));
                QPointF b = QPointF(time2Pixel(_selected_lines_temp[i].startTime), getYPixel(_selected_lines_temp[i].startY));

                auto result = MathHelper1::superEllipseYtoX(a, b, curve_n, curve_m, _windowRect.top());

                t1_temp = pixel2Time(result.x());

                if (t1_temp >= _selected_lines_temp[i].startTime && t1_temp <= _selected_lines_temp[i].endTime) {

                    float p, q, r, s, y;
                    bool rr = findNewRoot(_selected_lines_temp, i, pixel2Time(result.x()), p, q, r, s, y);

                    if (rr) {
                        new_curve_n_1 = p;
                        new_curve_m_1 = q;
                        t1 = t1_temp;
                        index1 = i;
                    } else {
                    }
                }
            } else {

                if (t1_temp >= _selected_lines_temp[i].startTime && t1_temp <= _selected_lines_temp[i].endTime) {
                    t1 = t1_temp;
                    index1 = i;
                    new_curve_n_1 = 1.0;
                    new_curve_m_1 = 1.0;
                }
            }
        }
        if (start_y < _windowRect.top() && end_y > _windowRect.top()) {
            auto t2_temp = getStartTime(0, _selected_lines_temp, i);

            if (curve_n != 1.0 || curve_n != 1.0) {
                QPointF a = QPointF(time2Pixel(_selected_lines_temp[i].endTime), getYPixel(_selected_lines_temp[i].endY));
                QPointF b = QPointF(time2Pixel(_selected_lines_temp[i].startTime), getYPixel(_selected_lines_temp[i].startY));

                auto result = MathHelper1::superEllipseYtoX(a, b, curve_n, curve_m, _windowRect.top());

                t2_temp = pixel2Time(result.x());

                if (t2_temp >= _selected_lines_temp[i].startTime && t2_temp <= _selected_lines_temp[i].endTime) {

                    float p, q, r, s, y;
                    bool rr = findNewRoot(_selected_lines_temp, i, pixel2Time(result.x()), p, q, r, s, y);

                    if (rr) {
                        new_curve_n_2 = r;
                        new_curve_m_2 = s;
                        t2 = t2_temp;
                        index2 = i;
                    } else {
                        continue;
                    }

                } else {
                    continue;
                }
            } else {

                if (t2_temp >= _selected_lines_temp[i].startTime && t2_temp <= _selected_lines_temp[i].endTime) {
                    t2 = t2_temp;
                    index2 = i;
                    new_curve_n_2 = 1.0;
                    new_curve_m_2 = 1.0;
                } else {
                    continue;
                }
            }

            double _startTime = _selected_lines_temp[index1].startTime;
            double startY = _selected_lines_temp[index1].startY;
            double _endTime = _selected_lines_temp[index2].endTime;
            double endY = _selected_lines_temp[index2].endY;

            if (index2 >= index1) {
                _selected_lines_temp_temp.append(
                    ALine { _startTime, t1, startY, 0, new_curve_n_1, new_curve_m_1 });

                _selected_lines_temp_temp.append(
                    ALine { t1, t2, 0,
                        0 });

                _selected_lines_temp_temp.append(
                    ALine { t2, _endTime, 0,
                        endY, new_curve_n_2, new_curve_m_2 });
            }
        }
        if (end_y >= _windowRect.top() && start_y >= _windowRect.top()) {
            _selected_lines_temp_temp.append(_selected_lines_temp[i]);
        }
    }
    qDebug() << "_selected_lines_temp_temp END";

    return _selected_lines_temp_temp;
}

QVector<Automation::ALine> Automation::streachDownChecking(QVector<ALine> _selected_lines_temp)
{

    QVector<ALine> _selected_lines_temp_temp;

    double t1, t2;
    int index1, index2;
    double new_curve_n_1 = 1.0;
    double new_curve_m_1 = 1.0;
    double new_curve_n_2 = 1.0;
    double new_curve_m_2 = 1.0;
    for (int i = 0; i < _selected_lines_temp.size(); i++) {

        int start_y = getYPixel(_selected_lines_temp[i].startY);
        int end_y = getYPixel(_selected_lines_temp[i].endY);
        double curve_n = _selected_lines_temp[i].curve_n;
        double curve_m = _selected_lines_temp[i].curve_m;

        if (end_y > _windowRect.bottom() && start_y < _windowRect.bottom()) {
            auto t1_temp = getStartTime((_windowRect.bottom() - _windowRect.top()) / (double)(_windowRect.bottom() - _windowRect.top()), _selected_lines_temp, i);

            if (curve_n != 1.0 || curve_n != 1.0) {
                QPointF a = QPointF(time2Pixel(_selected_lines_temp[i].endTime), getYPixel(_selected_lines_temp[i].endY));
                QPointF b = QPointF(time2Pixel(_selected_lines_temp[i].startTime), getYPixel(_selected_lines_temp[i].startY));

                auto result = MathHelper1::superEllipseYtoX(a, b, curve_n, curve_m, _windowRect.bottom());

                t1_temp = pixel2Time(result.x());

                if (t1_temp >= _selected_lines_temp[i].startTime && t1_temp <= _selected_lines_temp[i].endTime) {

                    float p, q, r, s, y;
                    bool rr = findNewRoot(_selected_lines_temp, i, pixel2Time(result.x()), p, q, r, s, y);

                    if (rr) {
                        new_curve_n_1 = p;
                        new_curve_m_1 = q;
                        t1 = t1_temp;
                        index1 = i;
                    } else {
                    }
                }
            } else {

                if (t1_temp >= _selected_lines_temp[i].startTime && t1_temp <= _selected_lines_temp[i].endTime) {
                    t1 = t1_temp;
                    index1 = i;
                    new_curve_n_1 = 1.0;
                    new_curve_m_1 = 1.0;
                }
            }
        }
        if (start_y > _windowRect.bottom() && end_y < _windowRect.bottom()) {

            auto t2_temp = getStartTime((_windowRect.bottom() - _windowRect.top()) / (double)(_windowRect.bottom() - _windowRect.top()), _selected_lines_temp, i);

            if (curve_n != 1.0 || curve_n != 1.0) {
                QPointF a = QPointF(time2Pixel(_selected_lines_temp[i].endTime), getYPixel(_selected_lines_temp[i].endY));
                QPointF b = QPointF(time2Pixel(_selected_lines_temp[i].startTime), getYPixel(_selected_lines_temp[i].startY));

                auto result = MathHelper1::superEllipseYtoX(a, b, curve_n, curve_m, _windowRect.bottom());

                t2_temp = pixel2Time(result.x());

                if (t2_temp >= _selected_lines_temp[i].startTime && t2_temp <= _selected_lines_temp[i].endTime) {

                    float p, q, r, s, y;
                    bool rr = findNewRoot(_selected_lines_temp, i, pixel2Time(result.x()), p, q, r, s, y);

                    if (rr) {
                        new_curve_n_2 = r;
                        new_curve_m_2 = s;
                        t2 = t2_temp;
                        index2 = i;
                    } else {
                        continue;
                    }

                } else {
                    continue;
                }
            } else {

                if (t2_temp >= _selected_lines_temp[i].startTime && t2_temp <= _selected_lines_temp[i].endTime) {
                    t2 = t2_temp;
                    index2 = i;
                    new_curve_n_2 = 1.0;
                    new_curve_m_2 = 1.0;
                } else {
                    continue;
                }
            }

            double _startTime = _selected_lines_temp[index1].startTime;
            double startY = _selected_lines_temp[index1].startY;
            double _endTime = _selected_lines_temp[index2].endTime;
            double endY = _selected_lines_temp[index2].endY;

            if (index2 >= index1) {
                _selected_lines_temp_temp.append(
                    ALine { _startTime, t1, startY, 1, new_curve_n_1, new_curve_m_1 });

                _selected_lines_temp_temp.append(
                    ALine { t1, t2, 1, 1 });

                _selected_lines_temp_temp.append(
                    ALine { t2, _endTime, 1,
                        endY, new_curve_n_2, new_curve_m_2 });
            }
        }
        if (end_y <= _windowRect.bottom() && start_y <= _windowRect.bottom()) {
            _selected_lines_temp_temp.append(_selected_lines_temp[i]);
        }
    }

    qDebug() << "_selected_lines_temp_temp END";
    return _selected_lines_temp_temp;
}

int Automation::getSelectedStartIndex() const
{
    return _selectedStartIndex;
}

int Automation::getSelectedEndIndex() const
{
    return _selectedEndIndex;
}

void Automation::setSelectedStartIndex(int selectedStartIndex)
{
    _selectedStartIndex = selectedStartIndex;
}

void Automation::setSelectedEndIndex(int selectedEndIndex)
{
    _selectedEndIndex = selectedEndIndex;
}

Automation::ALine Automation::getStartPoint()
{
    return _start_line;
}

void Automation::setStartPoint(const ALine& startPoint)
{
    _start_line = startPoint;
}

Automation::ALine Automation::getEndPoint()
{
    return _end_line;
}

void Automation::setEndPoint(const ALine& endPoint)
{
    _end_line = endPoint;
}

Automation::ALine Automation::getFirst_point()
{
    return _first_line;
}

void Automation::setFirst_point(const ALine& first_point)
{
    _first_line = first_point;
}

Automation::ALine Automation::getLast_point()
{
    return _last_line;
}

void Automation::setLast_point(const ALine& last_point)
{
    _last_line = last_point;
}

QVector<Automation::ALine> Automation::getFirst_list()
{
    return _first_list;
}

void Automation::setFirst_list(const QVector<ALine>& first_list)
{
    _first_list = first_list;
}

QVector<Automation::ALine> Automation::getLast_list()
{
    return _last_list;
}

void Automation::setLast_list(const QVector<ALine>& last_list)
{
    _last_list = last_list;
}

bool Automation::getInit_selected_line_move() const
{
    return _init_selected_line_move;
}

void Automation::setInit_selected_line_move(bool init_selected_line_move)
{
    _init_selected_line_move = init_selected_line_move;
}

QJsonArray Automation::copyList() const
{
    return _copiedLines.toJsonArray();
}

void Automation::setCopyList(const QJsonArray copyList)
{
    _copiedLines.fromJsonArray(copyList);
}

void Automation::clearCopyList()
{
    _copiedLines.clear();
}

QRect Automation::getWindowRect() const
{
    return _windowRect;
}

double Automation::getStartMarker() const
{
    return _startMarker;
}

double Automation::getEndMarker() const
{

    return _endMarker;
}

int Automation::getTopMarker() const
{
    return _topMarker;
}

int Automation::getBottomMarker() const
{
    return _bottomMarker;
}

bool Automation::enabled() const
{
    return _enabled;
}

void Automation::setEnabled1(bool enabled)
{
    if (enabled != _enabled) {
        _enabled = enabled;

        _cLines.setEnabled(enabled);
        Q_EMIT enabledChanged();
    }
}

int Automation::getSubTrackIndex() const
{
    return _subTrackIndex;
}

void Automation::setSubTrackIndex(int subTrackIndex)
{
    _subTrackIndex = subTrackIndex;
}

bool Automation::getAsyncClear() const
{
    return _asyncClear;
}

double Automation::automatedNormalized() const
{

    return _automatedNormalized;
}

void Automation::setAutomatedNormalized(double newAutomatedNormalized)
{

    _automatedNormalized = newAutomatedNormalized;
    Q_EMIT valueChanged();
}

double Automation::defaultNormalized() const
{
    return _defaultNormalized;
}

void Automation::setDefaultNormalized(double newDefaultNormalized)
{
    _defaultNormalized = newDefaultNormalized;
}

double Automation::normalized() const
{
    return _normalized;
}

void Automation::setNormalized(double newNormalized)
{

    if (_cLines.size() == 2) {
        _cLines.head()->setY(newNormalized);
        _cLines.tail()->setY(newNormalized);
    }
    _normalized = newNormalized;
    Q_EMIT valueChanged();
}

double Automation::minValue() const
{
    return _minValue;
}

void Automation::setMinValue(double newMinValue)
{
    _minValue = newMinValue;
}

double Automation::maxValue() const
{
    return _maxValue;
}

void Automation::setMaxValue(double newMaxValue)
{
    _maxValue = newMaxValue;
}

QString Automation::unit() const
{
    return _unit;
}

void Automation::setUnit(const QString& newUnit)
{
    _unit = newUnit;
}

int Automation::trackIndex() const
{
    return _trackIndex;
}

QString Automation::type() const
{
    return _type;
}

QString Automation::subType() const
{
    return _subType;
}

bool Automation::discreted() const
{
    return _discreted;
}

void Automation::setDiscreted(bool newDiscreted)
{
    _discreted = newDiscreted;
}

double Automation::getYPixel(double y)
{
    double H = (_windowRect.bottom() - _windowRect.top());
    double T = _windowRect.top();
    return y * H + T;
}

float Automation::minY() const
{
    return m_minY;
}

float Automation::maxY() const
{
    return m_maxY;
}

bool Automation::fixedVertical() const
{
    return _fixedVertical;
}

void Automation::setFixedVertical(bool newFixedVertical)
{
    _fixedVertical = newFixedVertical;
}

Automation::PointShape Automation::pointShape() const
{
    return _pointShape;
}

void Automation::setPointShape(PointShape newPointShape)
{
    _pointShape = newPointShape;
}

void Automation::reverseSelectedLines()
{
    QVector<ALine> temp;
    for (int i = 0; i < _selected_lines.size(); i++) {

        int index = _selected_lines.size() - i - 1;
        auto t = _selected_lines[index];

        t.rStartTime = 1 - _selected_lines[index].rEndTime;
        t.rEndTime = 1 - _selected_lines[index].rStartTime;
        t.startY = _selected_lines[index].endY;
        t.endY = _selected_lines[index].startY;

        t.baseStartY = _selected_lines[index].baseEndY;
        t.baseEndY = _selected_lines[index].baseStartY;

        if (_selected_lines[index].curve_n != 1.0) {
            t.curve_n = -_selected_lines[index].curve_n;
        }
        if (_selected_lines[index].curve_m != 1.0) {
            t.curve_m = -_selected_lines[index].curve_m;
        }

        t.reversed = !t.reversed;

        temp.append(t);
    }

    _selected_lines.clear();
    _selected_lines = temp;
}

void Automation::moveSelectedPoints(QPointF currentPos)
{

    if (_init_selected_point_move) {

        _init_selected_point_move = false;

        _selected_lines_temp.clear();
        _first_list.clear();
        _last_list.clear();

        _start_index = find_first_selected_lines_index(_selected_lines.first().startTime);
        _end_index = find_last_selected_lines_index(_selected_lines.last().endTime);

        qDebug() << "_end_index" << _start_index << _end_index << _selected_point_index;
        for (int i = 0; i <= _start_index; i++) {
            _first_list.append(_lines[i]);
        }

        for (int i = _end_index; i <= _lines.size() - 1; i++) {
            _last_list.append(_lines[i]);
        }

        for (int i = 0; i <= _first_list.size() - 1; i++) {
            _selected_lines_temp.append(_first_list[i]);
        }

        _selected_point.clear();

        if (_selected_point_index.first() >= 0) {

            _selected_lines_temp.append(ALine { _lines[_selected_point_index.first()].startTime, _lines[_selected_point_index.first()].endTime,
                _lines[_selected_point_index.first()].startY, _lines[_selected_point_index.first()].endY, _lines[_selected_point_index.first()].curve_n,
                _lines[_selected_point_index.first()].curve_m });

            if (_selected_lines.first().startTime > _lines[_selected_point_index.first()].startTime) {
            } else {
                _selected_point.append(ALine { _lines[_selected_point_index.first()].startTime, _lines[_selected_point_index.first()].endTime,
                    _lines[_selected_point_index.first()].startY, _lines[_selected_point_index.first()].endY, _lines[_selected_point_index.first()].curve_n,
                    _lines[_selected_point_index.first()].curve_m });
            }
        }

        for (int i = 1; i < _selected_point_index.size() - 1; i++) {

            _selected_lines_temp.append(ALine { _lines[_selected_point_index[i]].startTime, _lines[_selected_point_index[i]].endTime, _lines[_selected_point_index[i]].startY,
                _lines[_selected_point_index[i]].endY, _lines[_selected_point_index[i]].curve_n, _lines[_selected_point_index[i]].curve_m });
            _selected_point.append(ALine { _lines[_selected_point_index[i]].startTime, _lines[_selected_point_index[i]].endTime, _lines[_selected_point_index[i]].startY,
                _lines[_selected_point_index[i]].endY, _lines[_selected_point_index[i]].curve_n, _lines[_selected_point_index[i]].curve_m });
        }

        if (_selected_point_index.last() < _lines.size()) {

            _selected_lines_temp.append(ALine { _lines[_selected_point_index.last()].startTime, _lines[_selected_point_index.last()].endTime,
                _lines[_selected_point_index.last()].startY, _lines[_selected_point_index.last()].endY,
                _lines[_selected_point_index.last()].curve_n, _lines[_selected_point_index.last()].curve_m });

            if (_selected_lines.last().endTime < _lines[_selected_point_index.last()].endTime) {

            } else {

                _selected_point.append(ALine { _lines[_selected_point_index.last()].startTime, _lines[_selected_point_index.last()].endTime,
                    _lines[_selected_point_index.last()].startY, _lines[_selected_point_index.last()].endY,
                    _lines[_selected_point_index.last()].curve_n, _lines[_selected_point_index.last()].curve_m });
            }
        }

        for (int i = 0; i < _selected_point.size(); i++) {

            QPointF start_point = QPointF(time2Pixel(_selected_point[i].startTime), getYPixel(_selected_point[i].startY));
            double distance;
            double slope;
            int sign;

            MathHelper1::distanceAndSlope(currentPos, start_point, distance, slope, sign);

            _selected_line_positions.append(PolarPosition { distance, slope, sign });

            MathHelper1::distanceAndSlope(QPointF(time2Pixel(_selected_point[i].startTime), getYPixel(_selected_point[i].startY)),
                QPointF(time2Pixel(_selected_point[i].endTime), getYPixel(_selected_point[i].endY)), distance, slope, sign);

            _selected_point[i]._distance = distance;
            _selected_point[i]._slope = slope;
            _selected_point[i]._sign = sign;
        }

        _selected_lines.clear();
        for (int i = 1; i < _last_list.size(); i++) {
            _selected_lines_temp.append(_last_list[i]);
        }

    } else {

        for (int i = 0; i < _selected_point.size(); i++) {

            QPointF start_point = MathHelper1::secondPoint(currentPos, _selected_line_positions[i].distance, _selected_line_positions[i].slope, _selected_line_positions[i].sign);
            QPointF end_point = MathHelper1::secondPoint(start_point, _selected_point[i]._distance, _selected_point[i]._slope, _selected_point[i]._sign);

            _selected_point[i].startTime = MathHelper1::round3(pixel2Time(start_point.x()));
            _selected_point[i].endTime = MathHelper1::round3(pixel2Time(end_point.x()));
            _selected_point[i].startY = (start_point.y() - _windowRect.top()) / (double)(_windowRect.bottom() - _windowRect.top());
            _selected_point[i].endY = (end_point.y() - _windowRect.top()) / (double)(_windowRect.bottom() - _windowRect.top());

            if (getYPixel(_selected_point[i].startY) < _windowRect.top()) {
                _selected_point[i].startY = m_minY;
            } else if (getYPixel(_selected_point[i].startY) > _windowRect.bottom()) {
                _selected_point[i].startY = m_maxY;
            }

            if (getYPixel(_selected_point[i].endY) < _windowRect.top()) {
                _selected_point[i].endY = m_minY;
            } else if (getYPixel(_selected_point[i].endY) > _windowRect.bottom()) {
                _selected_point[i].endY = m_maxY;
            }
        }

        _selected_lines_temp.clear();

        int start_index = find_first_selected_lines_index(_selected_point.first().startTime);
        int end_index = find_last_selected_lines_index(_selected_point.last().endTime);

        if (_first_list.size() > 0 && _last_list.size() > 0) {

            if (_selected_point.first().startTime >= _first_list.last().startTime && _selected_point.first().startTime <= _last_list.first().endTime
                && _selected_point.last().endTime >= _first_list.last().startTime && _selected_point.last().endTime <= _last_list.first().endTime) {

                QVector<ALine> first_list;
                QVector<ALine> last_list;

                for (int i = 0; i <= _start_index - 1; i++) {
                    first_list.append(_lines[i]);
                }

                for (int i = _end_index + 1; i <= _lines.size() - 1; i++) {
                    last_list.append(_lines[i]);
                }

                if (first_list.size() > 0) {
                    for (int i = 0; i < first_list.size(); i++) {
                        _selected_lines_temp.append(first_list[i]);
                    }

                    if (_start_index >= 0 && _start_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { first_list.last().endTime, _selected_point.first().startTime,
                            first_list.last().endY, _selected_point.first().startY, _lines[_start_index].curve_n, _lines[_start_index].curve_m });
                    } else {
                        _selected_lines_temp.append(ALine { first_list.last().endTime, _selected_point.first().startTime,
                            first_list.last().endY, _selected_point.first().startY });
                    }

                } else {

                    if (start_index >= 0) {

                        if (_start_index >= 0 && _start_index < _lines.size()) {
                            _selected_lines_temp.append(ALine { _lines.first().startTime, _selected_point.first().startTime,
                                _lines.first().startY, _selected_point.first().startY, _lines[_start_index].curve_n, _lines[_start_index].curve_m });
                        } else {
                            _selected_lines_temp.append(ALine { _lines.first().startTime, _selected_point.first().startTime,
                                _lines.first().startY, _selected_point.first().startY });
                        }
                    }
                }

                for (int i = 0; i < _selected_point.size(); i++) {

                    _selected_lines_temp.append(ALine { _selected_point[i].startTime, _selected_point[i].endTime, _selected_point[i].startY, _selected_point[i].endY, _selected_point[i].curve_n, _selected_point[i].curve_m });
                }

                if (last_list.size() > 0) {

                    if (_end_index >= 0 && _end_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { _selected_point.last().endTime, last_list.first().startTime,
                            _selected_point.last().endY, last_list.first().startY,
                            _lines[_end_index].curve_n, _lines[_end_index].curve_m });
                    } else {
                        _selected_lines_temp.append(ALine { _selected_point.last().endTime, last_list.first().startTime,
                            _selected_point.last().endY, last_list.first().startY });
                    }
                    for (int i = 0; i < last_list.size(); i++) {
                        _selected_lines_temp.append(last_list[i]);
                    }
                } else {
                    if (end_index < _lines.size()) {

                        if (_end_index >= 0 && _end_index < _lines.size()) {
                            _selected_lines_temp.append(ALine { _selected_point.last().endTime, _lines.last().endTime,
                                _selected_point.last().endY, _lines.last().endY, _lines[_end_index].curve_n, _lines[_end_index].curve_m });
                        } else {
                            _selected_lines_temp.append(ALine { _selected_point.last().endTime, _lines.last().endTime,
                                _selected_point.last().endY, _lines.last().endY });
                        }
                    }
                }

            } else if (_selected_point.last().endTime <= _first_list.last().startTime) {

                QVector<ALine> first_list;
                QVector<ALine> last_list;

                for (int i = 0; i <= start_index - 1; i++) {
                    first_list.append(_lines[i]);
                }

                for (int i = end_index + 1; i <= _start_index - 1; i++) {
                    last_list.append(_lines[i]);
                }

                if (first_list.size() > 0) {

                    for (int i = 0; i < first_list.size(); i++) {
                        _selected_lines_temp.append(first_list[i]);
                    }

                    _selected_lines_temp.append(ALine { first_list.last().endTime, _selected_point.first().startTime,
                        first_list.last().endY, _selected_point.first().startY, _lines[start_index].curve_n, _lines[start_index].curve_m });

                } else {

                    if (start_index >= 0 && start_index < _lines.size())
                        _selected_lines_temp.append(ALine { _lines.first().startTime, _selected_point.first().startTime,
                            _lines.first().startY, _selected_point.first().startY, _lines[start_index].curve_n, _lines[start_index].curve_m });
                }

                for (int i = 0; i < _selected_point.size(); i++) {

                    _selected_lines_temp.append(ALine { _selected_point[i].startTime, _selected_point[i].endTime, _selected_point[i].startY, _selected_point[i].endY, _selected_point[i].curve_n, _selected_point[i].curve_m });
                }

                if (last_list.size() > 0) {
                    _selected_lines_temp.append(ALine { _selected_point.last().endTime, last_list.first().startTime,
                        _selected_point.last().endY, last_list.first().startY, _lines[_end_index].curve_n, _lines[_end_index].curve_m });
                    for (int i = 0; i < last_list.size(); i++) {
                        _selected_lines_temp.append(last_list[i]);
                    }
                } else {
                    _selected_lines_temp.append(ALine { _selected_point.last().endTime, _first_list.last().startTime,
                        _selected_point.last().endY, _first_list.last().startY, _lines[_end_index].curve_n, _lines[_end_index].curve_m });
                }

                _selected_lines_temp.append(ALine { _first_list.last().startTime, _last_list.first().endTime,
                    _first_list.last().startY, _last_list.first().endY, _lines[_end_index].curve_n, _lines[_end_index].curve_m });

                for (int i = 1; i < _last_list.size(); i++) {
                    _selected_lines_temp.append(_last_list[i]);
                }

            } else if (_selected_point.first().startTime >= _last_list.first().endTime) {

                QVector<ALine> first_list;
                QVector<ALine> last_list;

                for (int i = _end_index + 1; i <= start_index - 1; i++) {
                    first_list.append(_lines[i]);
                }

                for (int i = end_index + 1; i <= _lines.size() - 1; i++) {
                    last_list.append(_lines[i]);
                }

                for (int i = 0; i < _first_list.size() - 1; i++) {
                    _selected_lines_temp.append(_first_list[i]);
                }

                _selected_lines_temp.append(ALine { _first_list.last().startTime, _last_list.first().endTime,
                    _first_list.last().startY, _last_list.first().endY, _lines[_start_index].curve_n, _lines[_start_index].curve_m });

                if (first_list.size() > 0) {

                    for (int i = 0; i < first_list.size(); i++) {
                        _selected_lines_temp.append(first_list[i]);
                    }

                    float curve_n = 1;
                    float curve_m = 1;
                    if (start_index < _lines.size() && start_index > 0) {
                        curve_n = _lines[start_index].curve_n;
                        curve_m = _lines[start_index].curve_m;
                    }

                    _selected_lines_temp.append(ALine { first_list.last().endTime, _selected_point.first().startTime,
                        first_list.last().endY, _selected_point.first().startY, curve_n, curve_m });

                } else {

                    float curve_n = 1;
                    float curve_m = 1;
                    if (start_index < _lines.size() && start_index > 0) {
                        curve_n = _lines[start_index].curve_n;
                        curve_m = _lines[start_index].curve_m;
                    }

                    _selected_lines_temp.append(ALine { _last_list.first().endTime, _selected_point.first().startTime,
                        _last_list.first().endY, _selected_point.first().startY, curve_n, curve_m });
                }

                for (int i = 0; i < _selected_point.size(); i++) {

                    _selected_lines_temp.append(ALine { _selected_point[i].startTime, _selected_point[i].endTime,
                        _selected_point[i].startY, _selected_point[i].endY,
                        _selected_point[i].curve_n, _selected_point[i].curve_m });
                }

                if (last_list.size() > 0) {
                    _selected_lines_temp.append(ALine { _selected_lines_temp.last().endTime, last_list.first().startTime,
                        _selected_lines_temp.last().endY, last_list.first().startY, _lines[_end_index].curve_n, _lines[_end_index].curve_m });
                    for (int i = 0; i < last_list.size(); i++) {
                        _selected_lines_temp.append(last_list[i]);
                    }
                } else {

                    if (end_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { _selected_lines_temp.last().endTime, _lines.last().endTime,
                            _selected_lines_temp.last().endY, _lines.last().endY, _lines[_end_index].curve_n, _lines[_end_index].curve_m });
                    }
                }

            } else if (_selected_point.last().endTime > _first_list.last().startTime && _selected_point.last().endTime < _last_list.first().endTime) {

                QVector<ALine> first_list;

                for (int i = 0; i <= start_index - 1; i++) {
                    first_list.append(_lines[i]);
                }

                if (first_list.size() > 0) {
                    for (int i = 0; i < first_list.size(); i++) {
                        _selected_lines_temp.append(first_list[i]);
                    }

                    _selected_lines_temp.append(ALine { first_list.last().endTime, _selected_point.first().startTime,
                        first_list.last().endY, _selected_point.first().startY, _lines[_start_index].curve_n, _lines[_start_index].curve_m });
                } else {
                    if (start_index >= 0) {

                        if (_start_index >= 0 && _start_index < _lines.size()) {
                            _selected_lines_temp.append(ALine { _lines.first().startTime, _selected_point.first().startTime,
                                _lines.first().startY, _selected_point.first().startY, _lines[_start_index].curve_n, _lines[_start_index].curve_m });
                        } else {
                            _selected_lines_temp.append(ALine { _lines.first().startTime, _selected_point.first().startTime,
                                _lines.first().startY, _selected_point.first().startY });
                        }
                    }
                }

                for (int i = 0; i < _selected_point.size(); i++) {

                    _selected_lines_temp.append(ALine { _selected_point[i].startTime, _selected_point[i].endTime,
                        _selected_point[i].startY, _selected_point[i].endY,
                        _selected_point[i].curve_n, _selected_point[i].curve_m });
                }

                _selected_lines_temp.append(ALine { _selected_point.last().endTime, _last_list.first().endTime,
                    _selected_point.last().endY, _last_list.first().endY, _lines[_end_index].curve_n, _lines[_end_index].curve_m });

                for (int i = 1; i < _last_list.size(); i++) {
                    _selected_lines_temp.append(_last_list[i]);
                }

            } else if (_selected_point.first().startTime > _first_list.last().startTime && _selected_point.first().startTime < _last_list.first().endTime) {

                QVector<ALine> last_list;

                for (int i = end_index + 1; i <= _lines.size() - 1; i++) {
                    last_list.append(_lines[i]);
                }

                for (int i = 0; i < _first_list.size() - 1; i++) {
                    _selected_lines_temp.append(_first_list[i]);
                }

                _selected_lines_temp.append(ALine { _first_list.last().startTime, _selected_point.first().startTime,
                    _first_list.last().startY, _selected_point.first().startY, _lines[_start_index].curve_n, _lines[_start_index].curve_m });

                for (int i = 0; i < _selected_point.size(); i++) {

                    _selected_lines_temp.append(ALine { _selected_point[i].startTime, _selected_point[i].endTime, _selected_point[i].startY, _selected_point[i].endY, _selected_point[i].curve_n, _selected_point[i].curve_m });
                }

                if (last_list.size() > 0) {
                    _selected_lines_temp.append(ALine { _selected_point.last().endTime, last_list.first().startTime,
                        _selected_point.last().endY, last_list.first().startY, _lines[_end_index].curve_n, _lines[_end_index].curve_m });

                    for (int i = 0; i < last_list.size(); i++) {
                        _selected_lines_temp.append(last_list[i]);
                    }
                } else {
                    if (end_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { _selected_point.last().endTime, _lines.last().endTime,
                            _selected_point.last().endY, _lines.last().endY, _lines[_end_index].curve_n, _lines[_end_index].curve_m });
                    }
                }
            }
        } else if (_first_list.size() == 0 && _last_list.size() > 0) {

            if (_selected_point.last().endTime <= _last_list.first().endTime) {

                QVector<ALine> last_list;

                for (int i = _end_index + 1; i <= _lines.size() - 1; i++) {
                    last_list.append(_lines[i]);
                }

                for (int i = 0; i < _selected_point.size(); i++) {

                    _selected_lines_temp.append(ALine { _selected_point[i].startTime, _selected_point[i].endTime,
                        _selected_point[i].startY, _selected_point[i].endY, _selected_point[i].curve_n, _selected_point[i].curve_m });
                }

                if (last_list.size() > 0) {

                    if (_end_index >= 0 && _end_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { _selected_point.last().endTime, last_list.first().startTime,
                            _selected_point.last().endY, last_list.first().startY, _lines[_end_index].curve_n, _lines[_end_index].curve_m });
                    } else {
                        _selected_lines_temp.append(ALine { _selected_point.last().endTime, last_list.first().startTime,
                            _selected_point.last().endY, last_list.first().startY });
                    }
                } else {
                    if (end_index < _lines.size()) {

                        if (_end_index >= 0 && _end_index < _lines.size()) {
                            _selected_lines_temp.append(ALine { _selected_point.last().endTime, _lines.last().endTime,
                                _selected_point.last().endY, _lines.last().endY, _lines[_end_index].curve_n, _lines[_end_index].curve_m });
                        } else {
                            _selected_lines_temp.append(ALine { _selected_point.last().endTime, _lines.last().endTime,
                                _selected_point.last().endY, _lines.last().endY });
                        }
                    }
                }

                for (int i = 0; i < last_list.size(); i++) {
                    _selected_lines_temp.append(last_list[i]);
                }
            } else if (_selected_point.first().startTime > _last_list.first().endTime) {
                QVector<ALine> first_list;
                QVector<ALine> last_list;

                for (int i = _end_index + 1; i <= start_index - 1; i++) {
                    first_list.append(_lines[i]);
                }

                for (int i = end_index + 1; i <= _lines.size() - 1; i++) {
                    last_list.append(_lines[i]);
                }

                if (first_list.size() > 0) {
                    for (int i = 0; i < first_list.size(); i++) {
                        _selected_lines_temp.append(first_list[i]);
                    }

                    if (_start_index >= 0 && _start_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { first_list.last().endTime, _selected_point.first().startTime,
                            first_list.last().endY, _selected_point.first().startY, _lines[_start_index].curve_n, _lines[_start_index].curve_m });
                    } else {
                        _selected_lines_temp.append(ALine { first_list.last().endTime, _selected_point.first().startTime,
                            first_list.last().endY, _selected_point.first().startY });
                    }
                } else {

                    if (_start_index >= 0 && _start_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { _last_list.first().endTime, _selected_point.first().startTime,
                            _last_list.first().endY, _selected_point.first().startY, _lines[_start_index].curve_n, _lines[_start_index].curve_m });
                    } else {
                        _selected_lines_temp.append(ALine { _last_list.first().endTime, _selected_point.first().startTime,
                            _last_list.first().endY, _selected_point.first().startY });
                    }
                }

                for (int i = 0; i < _selected_point.size(); i++) {

                    _selected_lines_temp.append(ALine { _selected_point[i].startTime, _selected_point[i].endTime,
                        _selected_point[i].startY, _selected_point[i].endY, _selected_point[i].curve_n, _selected_point[i].curve_m });
                }

                if (last_list.size() > 0) {
                    if (_end_index >= 0 && _end_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { _selected_point.last().endTime, last_list.first().startTime,
                            _selected_point.last().endY, last_list.first().startY, _lines[_end_index].curve_n, _lines[_end_index].curve_m });
                    } else {
                        _selected_lines_temp.append(ALine { _selected_point.last().endTime, last_list.first().startTime,
                            _selected_point.last().endY, last_list.first().startY });
                    }
                }

                for (int i = 0; i < last_list.size(); i++) {
                    _selected_lines_temp.append(last_list[i]);
                }
            } else {

                QVector<ALine> first_list;
                QVector<ALine> last_list;

                for (int i = _end_index + 1; i <= start_index - 1; i++) {
                    first_list.append(_lines[i]);
                }

                for (int i = end_index + 1; i <= _lines.size() - 1; i++) {
                    last_list.append(_lines[i]);
                }

                for (int i = 0; i < _selected_point.size(); i++) {

                    _selected_lines_temp.append(ALine { _selected_point[i].startTime, _selected_point[i].endTime,
                        _selected_point[i].startY, _selected_point[i].endY,
                        _selected_point[i].curve_n, _selected_point[i].curve_m });
                }

                if (last_list.size() > 0) {

                    if (_end_index >= 0 && _end_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { _selected_point.last().endTime, last_list.first().startTime,
                            _selected_point.last().endY, last_list.first().startY, _lines[_end_index].curve_n, _lines[_end_index].curve_m });
                    } else {
                        _selected_lines_temp.append(ALine { _selected_point.last().endTime, last_list.first().startTime,
                            _selected_point.last().endY, last_list.first().startY });
                    }
                }

                for (int i = 0; i < last_list.size(); i++) {
                    _selected_lines_temp.append(last_list[i]);
                }
            }
        } else if (_first_list.size() > 0 && _last_list.size() == 0) {

            if (_selected_point.first().startTime >= _first_list.last().startTime) {
                QVector<ALine> first_list;

                for (int i = 0; i <= _start_index - 1; i++) {
                    first_list.append(_lines[i]);
                }

                for (int i = 0; i < first_list.size(); i++) {
                    _selected_lines_temp.append(first_list[i]);
                }

                if (first_list.size() > 0) {

                    if (_start_index >= 0 && _start_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { first_list.last().endTime, _selected_point.first().startTime,
                            first_list.last().endY, _selected_point.first().startY, _lines[_start_index].curve_n, _lines[_start_index].curve_m });
                    } else {
                        _selected_lines_temp.append(ALine { first_list.last().endTime, _selected_point.first().startTime,
                            first_list.last().endY, _selected_point.first().startY });
                    }
                } else {
                    if (_start_index >= 0 && _start_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { _lines.first().startTime, _selected_point.first().startTime,
                            _lines.first().startY, _selected_point.first().startY, _lines[_start_index].curve_n, _lines[_start_index].curve_m });
                    }
                }

                for (int i = 0; i < _selected_point.size(); i++) {

                    _selected_lines_temp.append(ALine { _selected_point[i].startTime, _selected_point[i].endTime,
                        _selected_point[i].startY, _selected_point[i].endY, _selected_point[i].curve_n, _selected_point[i].curve_m });
                }

            } else if (_selected_point.last().endTime < _first_list.last().startTime) {

                QVector<ALine> first_list;
                QVector<ALine> last_list;

                for (int i = 0; i <= start_index - 1; i++) {
                    first_list.append(_lines[i]);
                }

                for (int i = end_index + 1; i <= _start_index - 1; i++) {
                    last_list.append(_lines[i]);
                }

                if (first_list.size() > 0) {
                    for (int i = 0; i < first_list.size(); i++) {
                        _selected_lines_temp.append(first_list[i]);
                    }

                    if (_start_index >= 0 && _start_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { first_list.last().endTime, _selected_point.first().startTime,
                            first_list.last().endY, _selected_point.first().startY, _lines[_start_index].curve_n, _lines[_start_index].curve_m });

                    } else {
                        _selected_lines_temp.append(ALine { first_list.last().endTime, _selected_point.first().startTime,
                            first_list.last().endY, _selected_point.first().startY });
                    }

                } else {
                }

                for (int i = 0; i < _selected_point.size(); i++) {

                    _selected_lines_temp.append(ALine { _selected_point[i].startTime, _selected_point[i].endTime,
                        _selected_point[i].startY, _selected_point[i].endY, _selected_point[i].curve_n, _selected_point[i].curve_m });
                }

                if (last_list.size() > 0) {

                    if (_end_index >= 0 && _end_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { _selected_point.last().endTime, last_list.first().startTime,
                            _selected_point.last().endY, last_list.first().startY, _lines[_end_index].curve_n, _lines[_end_index].curve_m });
                    } else {
                        _selected_lines_temp.append(ALine { _selected_point.last().endTime, last_list.first().startTime,
                            _selected_point.last().endY, last_list.first().startY });
                    }

                    for (int i = 0; i < last_list.size(); i++) {
                        _selected_lines_temp.append(last_list[i]);
                    }
                } else {

                    if (_end_index >= 0 && _end_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { _selected_point.last().endTime, _first_list.last().startTime,
                            _selected_point.last().endY, _first_list.last().startY,
                            _lines[_end_index].curve_n, _lines[_end_index].curve_m });
                    } else {
                        _selected_lines_temp.append(ALine { _selected_point.last().endTime, _first_list.last().startTime,
                            _selected_point.last().endY, _first_list.last().startY });
                    }
                }

            } else {
                QVector<ALine> first_list;
                QVector<ALine> last_list;

                for (int i = 0; i <= start_index - 1; i++) {
                    first_list.append(_lines[i]);
                }

                for (int i = 0; i < first_list.size(); i++) {
                    _selected_lines_temp.append(first_list[i]);
                }

                if (first_list.size() > 0) {

                    if (_start_index >= 0 && _start_index < _lines.size()) {
                        _selected_lines_temp.append(ALine { first_list.last().endTime, _selected_point.first().startTime,
                            first_list.last().endY, _selected_point.first().startY, _lines[_start_index].curve_n, _lines[_start_index].curve_m });
                    } else {
                        _selected_lines_temp.append(ALine { first_list.last().endTime, _selected_point.first().startTime,
                            first_list.last().endY, _selected_point.first().startY });
                    }
                }

                for (int i = 0; i < _selected_point.size(); i++) {

                    _selected_lines_temp.append(ALine { _selected_point[i].startTime, _selected_point[i].endTime,
                        _selected_point[i].startY, _selected_point[i].endY, _selected_point[i].curve_n, _selected_point[i].curve_m });
                }
            }
        } else {

            for (int i = 0; i < _selected_point.size(); i++) {
                _selected_lines_temp.append(ALine { _selected_point[i].startTime, _selected_point[i].endTime, _selected_point[i].startY, _selected_point[i].endY, _selected_point[i].curve_n, _selected_point[i].curve_m });
            }
        }
    }
}
void Automation::clearMovingLine()
{
    _selected_for_moving_line.clear();
}

QVector<Automation::ALine> Automation::cloneLines()
{
    if (_selected_lines_temp.size() > 0) {
        return _selected_lines_temp;
    }
    if (_temp_lines.size() > 0) {
        return _temp_lines;
    }
    return _lines;
}

void Automation::setSelectedLines(QVector<Automation::ALine> lines, int startIndex, int endIndex)
{

    if (lines.size() == 0)
        return;

    _selected_lines.clear();
    _selected_lines = lines;

    for (int i = startIndex + 1; i <= endIndex; i++) {
        _init_selected_point_move = true;

        _selected_point_index.append(i);
    }

    for (int i = 1; i < _selected_lines.size() - 1; i++) {

        _selected_point2.append(APoint { _selected_lines[i].startTime, _selected_lines[i].startY });
    }

    _selected_point2.append(APoint { _selected_lines.last().startTime, _selected_lines.last().startY });
}

QVector<Automation::ALine> Automation::cloneSelectedLines()
{

    if (_selected_lines.size() > 0) {
        return _selected_lines;
    }
    return QVector<Automation::ALine>();
}

bool Automation::getLimitUpStreach() const
{
    return _limitUpStreach;
}

void Automation::setLimitUpStreach(bool limitUpStreach)
{
    _limitUpStreach = limitUpStreach;
}

bool Automation::getLimitDownStreach() const
{
    return _limitDownStreach;
}

void Automation::setLimitDownStreach(bool limitDownStreach)
{
    _limitDownStreach = limitDownStreach;
}

void Automation::clearSelectedLines()
{

    _selected_lines.clear();
    _selected_line_positions.clear();
    _init_selected_line_move = false;
    _init_selected_point_move = true;
    _temp_lines.clear();
    _selected_lines_temp.clear();
    _selected_point_index.clear();
}
