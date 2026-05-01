#ifndef AUTOMATION_H
#define AUTOMATION_H

#include "mathhelper.h"
#include "warp.h"
#include <QColor>
#include <QJsonObject>
#include <QLine>
#include <QList>
#include <QtMath>
#include <core/global.h>
class TrackItem;
class AreaInfo;

#pragma once

#include <QtCore/QPointF>
#include <QtGui/QColor>
#include <limits>

class CPoint {
public:
    // --- Constructors -------------------------------------------------------
    CPoint() = default;

    // this enum is for specify two point of first and last when we want to move a group of points in selectedLines
    // beacuse they have different behave
    enum MoveTagState {
        MoveTagState_None = 0,
        MoveTagState_First,
        MoveTagState_Last,
    };

    CPoint(double t,
        double yVal,

        const QPointF& position = {}, MoveTagState moveTag = MoveTagState_None)
        : m_time(t)
        , m_y(yVal)
        , m_moveTag(moveTag)
    {
        _effectiveTime = t;
    }

    CPoint* clone() const
    {
        auto* p = new CPoint();

        p->m_time = m_time;
        p->m_y = m_y;

        p->m_hovred = m_hovred;
        p->_curve_n = _curve_n;
        p->_curve_m = _curve_m;
        p->m_moveTag = m_moveTag;

        // Do NOT copy links by default
        p->m_next = nullptr;
        p->m_prev = nullptr;

        return p;
    }

    // --- Factory: -∞ and +∞ points ----------------------------------------
    static CPoint* NegInf(double yVal = 0.0)
    {
        auto p = new CPoint();
        p->m_time = -std::numeric_limits<double>::infinity();
        p->m_y = yVal;
        return p;
    }

    static CPoint* PosInf(double yVal = 0.0)
    {
        auto p = new CPoint();
        p->m_time = std::numeric_limits<double>::infinity();
        p->m_y = yVal;
        return p;
    }

    bool isValid()
    {
        if (next() && prev()) {
            return true;
        }
        return false;
    }

    // --- Getters / Setters --------------------------------------------------
    double time() const { return m_time; }
    double y() const { return m_y; }
    QPointF pos() { return QPointF(m_time, m_y); }

    void setTime(double t)
    {
        m_time = t;
        _effectiveTime = t;
    }
    void setY(double yVal) { m_y = yVal; }

    // --- Doubly-linked list API --------------------------------------------
    CPoint* next() const { return m_next; }
    CPoint* prev() const { return m_prev; }

    void setNext(CPoint* p) { m_next = p; }
    void setPrev(CPoint* p) { m_prev = p; }

    bool isNegInf()
    {
        if (std::isinf(time()) && std::signbit(time())) {
            return true;
        }
        return false;
    }

    bool isPosInf()
    {
        if (std::isinf(time()) && !std::signbit(time())) {
            return true;
        }
        return false;
    }

    bool hovred() { return m_hovred; }
    void setHovred(bool c) { m_hovred = c; }

    bool enabled() { return m_enabled; }
    void setEnbaled(bool c) { m_enabled = c; }

    static QJsonObject pointToJson(CPoint* p)
    {
        QJsonObject obj;

        obj["time"] = p->time();
        obj["y"] = p->y();
        obj["hovred"] = p->hovred();

        obj["curve_n"] = p->curve_n();
        obj["curve_m"] = p->curve_m();

        // If you want to preserve sentinels instead of skipping them:
        obj["isNegInf"] = p->isNegInf();
        obj["isPosInf"] = p->isPosInf();

        obj["moveTag"] = p->moveTag();

        return obj;
    }

    static CPoint* pointFromJson(const QJsonObject& obj)
    {
        const double t = obj["time"].toDouble();
        const double y = obj["y"].toDouble();

        const double cn = obj["curve_n"].toDouble();
        const double cm = obj["curve_m"].toDouble();

        auto* p = new CPoint(t, y);

        p->setCurve_m(cm);
        p->setCurve_n(cn);
        p->setMoveTag(MoveTagState(obj["moveTag"].toInt()));

        // If you want to reconstruct +/-∞, you can override t here
        if (obj.value("isNegInf").toBool())
            p->setTime(-std::numeric_limits<double>::infinity());
        else if (obj.value("isPosInf").toBool())
            p->setTime(std::numeric_limits<double>::infinity());

        // Links (next/prev) are NOT set here; CPointList will handle that.
        return p;
    }

    double curve_n() const
    {
        return _curve_n;
    }

    void setCurve_n(double newCurve_n)
    {
        _curve_n = newCurve_n;
    }

    double curve_m() const
    {
        return _curve_m;
    }

    void setCurve_m(double newCurve_m)
    {
        _curve_m = newCurve_m;
    }

    MoveTagState moveTag() const
    {
        return m_moveTag;
    }

    void setMoveTag(MoveTagState tag)
    {
        m_moveTag = tag;
    }

    double effectiveTime() const
    {
        return _effectiveTime;
    }

    void setEffectiveTime(double time)
    {
        _effectiveTime = time;
    }

private:
    double m_time = -1.0;
    double m_y = -1.0;

    double _curve_n = 1.0;
    double _curve_m = 1.0;

    // this value specify double effective time value when we are selecting a period inside superellipse
    double _effectiveTime = -1;

    // --- New: pointers for linked list -------------------------------------
    CPoint* m_next = nullptr;
    CPoint* m_prev = nullptr;

    bool m_hovred = false;

    bool m_enabled = true;
    // this is for when we want to move a group of points or lines togather
    // first and las point should be tagged
    MoveTagState m_moveTag = MoveTagState_None;

    // QColor m_color = QColor { QStringLiteral("#00ff66") };
};

class CLine {
public:
    // --- Constructors -------------------------------------------------------
    CLine() = default;

    CLine(CPoint* p1,
        CPoint* p2)
        : m_p1(p1)
        , m_p2(p2)
    {
    }
    // --- Getters / Setters --------------------------------------------------
    CPoint* p1() const { return m_p1; }
    CPoint* p2() const { return m_p2; }

    void setP1(CPoint* p) { m_p1 = p; }
    void setP2(CPoint* p) { m_p2 = p; }

    double curveN() const { return m_curveN; }
    double curveM() const { return m_curveM; }

    void setCurveN(double v) { m_curveN = v; }
    void setCurveM(double v) { m_curveM = v; }

    bool hovred() { return m_hovred; }
    void setHovred(bool c) { m_hovred = c; }

    bool enabled() { return m_enabled; }
    void setEnabled(bool c) { m_enabled = c; }

    const QPointF& fp1() const { return m_fp1; }
    const QPointF& fp2() const { return m_fp2; }

    void setFp1(const QPointF& p) { m_fp1 = p; }
    void setFp2(const QPointF& p) { m_fp2 = p; }

    double startX() { return m_startX; }
    void setStartX(double c) { m_startX = c; }

    double endX() { return m_endX; }
    void setEndX(double c) { m_endX = c; }

    bool isValid()
    {
        if (m_p1 && m_p2) {
            return true;
        }
        return false;
    }

    std::pair<double, double> minMaxY() const
    {
        if (!m_p1 || !m_p2) {
            return { 0.0, 0.0 }; // or throw, or assert(false)
        }

        const double y1 = m_p1->y();
        const double y2 = m_p2->y();

        // Option A: manual
        // return { std::min(y1, y2), std::max(y1, y2) };

        // Option B: using std::minmax
        auto mm = std::minmax(y1, y2);
        return { mm.first, mm.second };
    }

private:
    CPoint* m_p1 = nullptr;
    CPoint* m_p2 = nullptr;

    double m_curveN = 1.0;
    double m_curveM = 1.0;

    QPointF m_fp1 {}; // (0,0)
    QPointF m_fp2 {}; // (0,0)

    double m_startX;
    double m_endX;

    bool m_hovred = false;
    bool m_enabled = true;
    // QColor m_color = QColor { QStringLiteral("#FFA500") };
};

class CPointList {
public:
    CPointList() = default;

    // non-copyable to avoid double-free by accident

    ~CPointList()
    {
        clear();
    }

    // --- deep-copy constructor --------------------------------------------
    CPointList(const CPointList& other)
    {
        copyFrom(other);
    }

    // --- deep-copy assignment ---------------------------------------------
    CPointList& operator=(const CPointList& other)
    {
        if (this != &other) {
            clear();
            copyFrom(other);
        }
        return *this;
    }

    // --- move constructor --------------------------------------------------
    CPointList(CPointList&& other) noexcept
        : m_head(other.m_head)
        , m_tail(other.m_tail)
    {
        other.m_head = nullptr;
        other.m_tail = nullptr;
    }

    // --- move assignment ---------------------------------------------------
    CPointList& operator=(CPointList&& other) noexcept
    {
        if (this != &other) {
            clear();
            m_head = other.m_head;
            m_tail = other.m_tail;
            other.m_head = nullptr;
            other.m_tail = nullptr;
        }
        return *this;
    }

    QJsonArray toJsonArray() const
    {
        QJsonArray arr;

        for (CPoint* cur = m_head; cur; cur = cur->next()) {

            arr.append(CPoint::pointToJson(cur));
        }

        return arr;
    }

    void fromJsonArray(const QJsonArray& arr)
    {
        // 1. Clear current list
        clear(); // implement: delete all nodes, set m_head/m_tail = nullptr

        CPoint* prev = nullptr;

        for (const QJsonValue& v : arr) {
            if (!v.isObject())
                continue;

            QJsonObject obj = v.toObject();
            CPoint* p = CPoint::pointFromJson(obj);
            if (!p)
                continue;

            // 2. Link into list
            if (!m_head) {
                m_head = m_tail = p;
            } else {
                prev->setNext(p);
                p->setPrev(prev);
                m_tail = p;
            }

            prev = p;
        }
    }

    void copyFrom(const CPointList& other)
    {
        for (CPoint* p = other.m_head; p; p = p->next()) {
            CPoint* np = new CPoint(p->time(), p->y(), p->pos());
            np->setCurve_m(p->curve_m());
            np->setCurve_n(p->curve_n());
            np->setHovred(p->hovred());
            np->setEnbaled(p->enabled());
            push_back(np);
        }
    }
    void setEnabled(bool e)
    {
        for (CPoint* p = m_head; p; p = p->next()) {

            p->setEnbaled(e);
            // p->setColor("#FFA500");
        }
    }

    void clearHovered()
    {
        for (CPoint* p = m_head; p; p = p->next()) {

            p->setHovred(false);
            // p->setColor("#FFA500");
        }
    }
    void clearMoveTag()
    {
        for (CPoint* p = m_head; p; p = p->next()) {

            p->setMoveTag(CPoint::MoveTagState_None);
            // p->setColor("#FFA500");
        }
    }

    void setAllHovered(bool hovered)
    {
        for (CPoint* p = m_head; p; p = p->next()) {
            p->setHovred(hovered);
        }
    }

    void translate(double dt, double dy)
    {
        for (CPoint* p = m_head; p != nullptr; p = p->next()) {
            // Optional: skip ±∞ sentinel points, if you use them as anchors
            if (p->isNegInf() || p->isPosInf()) {
                continue;
            }

            p->setTime(p->time() + dt);
            p->setY(p->y() + dy);
        }
    }

    static CPointList buildListFromSegments(const std::vector<std::pair<CPoint*, CPoint*>>& segments)
    {
        CPointList newList;

        if (segments.empty())
            return newList;

        // --- extract points in correct order ---
        std::vector<CPoint*> ordered;
        ordered.reserve(segments.size() + 1);

        // First segment gives us the start
        ordered.push_back(segments.front().first);

        for (auto& seg : segments) {
            ordered.push_back(seg.second);
        }

        // --- build the new list with cloned points ---
        for (CPoint* p : ordered) {
            CPoint* np = new CPoint(p->time(), p->y(), p->pos());
            np->setCurve_m(p->curve_m());
            np->setCurve_n(p->curve_n());
            np->setEnbaled(p->enabled());

            if (p->y() < 0) {
                np->setY(0);
            }
            if (p->y() > 1) {
                np->setY(1);
            }
            np->setHovred(p->hovred());
            newList.push_back(np);
        }

        return newList;
    }

    std::vector<std::pair<CPoint*, CPoint*>>
    segmentsToPoint(CPoint* point) const
    {
        std::vector<std::pair<CPoint*, CPoint*>> segments;
        if (!point || !m_head)
            return segments;

        CPoint* prev = m_head;

        // point is the head itself → no segments
        if (prev == point)
            return segments;

        for (CPoint* cur = prev->next(); cur; cur = cur->next()) {
            segments.emplace_back(prev, cur);
            if (cur == point)
                break;
            prev = cur;
        }

        return segments;
    }

    void extractRange(CPointList* result, CPoint* start, CPoint* end)
    {
        if (!result)
            return;

        result->clear();

        if (!start || !end /*|| start == end*/)
            return;

        // --------------------------------------------------------
        // Ensure start comes before end in the list
        // --------------------------------------------------------
        CPoint* first = start;
        CPoint* last = end;

        bool inOrder = false;
        for (CPoint* p = first; p; p = p->next()) {
            if (p == last) {
                inOrder = true;
                break;
            }
        }
        if (!inOrder) {
            std::swap(first, last);
        }

        // --------------------------------------------------------
        // Save pointers BEFORE and AFTER the range
        // --------------------------------------------------------
        CPoint* before = first->prev();
        CPoint* after = last->next();

        // --------------------------------------------------------
        // Move nodes [first .. last] into result
        // --------------------------------------------------------
        CPoint* cur = first;
        while (cur) {
            CPoint* next = cur->next(); // save original next before we touch links
            result->push_clone(cur);
            // detach from original chain (internally)
            cur->setPrev(nullptr);
            cur->setNext(nullptr);

            delete cur;

            if (cur == last)
                break;

            cur = next;
        }

        // --------------------------------------------------------
        // Fix original list: connect before <-> after
        // --------------------------------------------------------
        if (before)
            before->setNext(after);
        else
            m_head = after; // first was the head

        if (after)
            after->setPrev(before);
        else {
            m_tail = before; // last was the tail
            // push_back(CPoint::PosInf(m_tail->y()));
        }

        // this part commented for QD-1464
        //         if (m_tail->isPosInf()) {
        //             if (m_tail && m_tail->prev()) {
        //                 m_tail->setY(m_tail->prev()->y());
        //             }
        //         }

        //        if (m_head->isNegInf()) {
        //            if (m_head && m_head->next()) {
        //                m_head->setY(m_head->next()->y());
        //            }
        //        }
    }

    void cloneRange(CPointList* result, CPoint* start, CPoint* end)
    {
        if (!result)
            return;

        result->clear();

        if (!start || !end)
            return;

        // --------------------------------------------------------
        // Ensure start comes before end in the list
        // --------------------------------------------------------
        CPoint* first = start;
        CPoint* last = end;

        bool inOrder = false;
        for (CPoint* p = first; p; p = p->next()) {
            if (p == last) {
                inOrder = true;
                break;
            }
        }
        if (!inOrder) {
            std::swap(first, last);
        }

        // --------------------------------------------------------
        // Copy nodes [first .. last] into result (clone only)
        // Original list is NOT modified.
        // --------------------------------------------------------
        CPoint* cur = first;
        while (cur) {
            result->push_clone(cur); // creates a NEW CPoint in result

            if (cur == last)
                break;

            cur = cur->next();
        }
    }

    std::vector<std::pair<CPoint*, CPoint*>>
    segmentsFromPoint(CPoint* point) const
    {
        std::vector<std::pair<CPoint*, CPoint*>> segments;
        if (!point)
            return segments;

        CPoint* prev = point;
        CPoint* cur = point->next();

        while (cur) {
            segments.emplace_back(prev, cur);
            prev = cur;
            cur = cur->next();
        }

        return segments;
    }

    std::vector<std::pair<CPoint*, CPoint*>> buildSegmentsWithInsertedList(CPoint* start, CPoint* end, CPointList* insertList) const
    {
        std::vector<std::pair<CPoint*, CPoint*>> segments;

        if (!start && !end) {
            return segments;
        }
        if (!m_head)
            return segments;

        std::vector<CPoint*> chain;

        chain.reserve(64); // arbitrary, avoids frequent reallocs

        // --------------------------------------------------------
        // 1) Prefix: from head up to 'before' (inclusive)
        // --------------------------------------------------------
        if (start) {
            for (CPoint* p = m_head; p; p = p->next()) {
                chain.push_back(p);
                if (p == start)
                    break;
            }
        } else {
            // Region starts at head → no prefix
        }

        // --------------------------------------------------------
        // 2) Middle: all points from insertList (in order)
        // --------------------------------------------------------
        for (CPoint* p = insertList->head(); p; p = p->next()) {
            chain.push_back(p);
        }

        // --------------------------------------------------------
        // 3) Suffix: from 'after' to tail (if any)
        // --------------------------------------------------------
        if (end) {
            for (CPoint* p = end; p; p = p->next()) {
                chain.push_back(p);
            }
        }

        // --------------------------------------------------------
        // Now turn chain of points into consecutive segments:
        // (p0,p1), (p1,p2), (p2,p3), ...
        // --------------------------------------------------------
        if (chain.size() < 2)
            return segments;

        segments.reserve(chain.size() - 1);
        for (std::size_t i = 1; i < chain.size(); ++i) {
            segments.emplace_back(chain[i - 1], chain[i]);
        }

        return segments;
    }

    std::vector<std::pair<CPoint*, CPoint*>> enforceMonotoneFromBaseForward(CPoint* base) const
    {
        std::vector<std::pair<CPoint*, CPoint*>> segments;

        CPoint* prev = base;
        CPoint* cur = base->next();

        while (cur) {
            // enforce: prev->time() < cur->time()
            if (cur->time() >= prev->time()) {
                segments.emplace_back(prev, cur);
                prev = cur; // accept cur as new “last” point
            }
            // else: skip cur (logically "removed")

            cur = cur->next();
        }

        return segments;
    }

    std::vector<std::pair<CPoint*, CPoint*>>
    enforceMonotoneFromBaseBackward(CPoint* base) const
    {
        std::vector<std::pair<CPoint*, CPoint*>> segments;

        CPoint* next = base; // “right” end of segment
        CPoint* cur = base->prev();

        // Go leftwards and enforce: cur->time() < next->time()
        // (so in left→right order it’s still increasing)
        while (cur) {
            if (cur->time() <= next->time()) {
                // valid step in a monotone chain: cur -> next
                segments.emplace_back(cur, next);
                next = cur; // extend chain to the left
            }
            // else: skip cur (logically "removed")

            cur = cur->prev();
        }

        // Now segments are from near-base outward to the left.
        // If you want them in head→tail order, reverse them:
        std::reverse(segments.begin(), segments.end());

        return segments;
    }

    CPointList clone() const
    {
        CPointList newList;

        for (CPoint* p = m_head; p; p = p->next()) {
            CPoint* np = new CPoint(p->time(), p->y(), p->pos());

            np->setCurve_m(p->curve_m());
            np->setCurve_n(p->curve_n());

            np->setEnbaled(p->enabled());

            np->setHovred(p->hovred());
            // note: CLine* links are NOT cloned here, only basic data
            newList.push_back(np);
        }

        return newList; // safe: deep copy; will use move / NRVO
    }

    std::vector<std::pair<CPoint*, CPoint*>> toPairs() const
    {
        std::vector<std::pair<CPoint*, CPoint*>> out;
        out.reserve(32);

        if (!m_head || !m_head->next())
            return out; // not enough points

        CPoint* p0 = m_head;
        CPoint* p1 = p0->next();

        while (p1) {
            out.emplace_back(p0, p1);
            p0 = p1;
            p1 = p1->next();
        }

        return out;
    }

    std::vector<CPoint*> toList() const
    {
        std::vector<CPoint*> out;

        for (CPoint* p = m_head; p; p = p->next())
            out.push_back(p);

        return out;
    }

    template <typename Predicate>
    std::vector<CPoint*> findAllIf(Predicate pred) const
    {
        std::vector<CPoint*> out;
        out.reserve(16); // optional optimization

        for (CPoint* p = m_head; p; p = p->next()) {
            if (pred(p))
                out.push_back(p);
        }

        return out;
    }

    static std::vector<CLine> makeConnectedLines(const std::vector<CPoint*>& points)
    {
        std::vector<CLine> lines;

        if (points.size() < 2)
            return lines;

        lines.reserve(points.size() - 1);

        for (std::size_t i = 1; i < points.size(); ++i) {
            CPoint* a = points[i - 1];
            CPoint* b = points[i];

            if (!a || !b)
                continue;

            // only if they are neighbors in the list
            if (a->next() != b && b->prev() != a)
                continue;

            CLine line = CLine(a, b);

            lines.push_back(line);
        }

        return lines;
    }

    // =====================================================================
    // Basic insertion
    // =====================================================================
    void push_front(CPoint* p)
    {
        if (!p)
            return;

        p->setPrev(nullptr);
        p->setNext(m_head);

        if (m_head)
            m_head->setPrev(p);
        else
            m_tail = p; // list was empty

        m_head = p;
    }

    void push_back(CPoint* p)
    {
        if (!p)
            return;

        p->setNext(nullptr);
        p->setPrev(m_tail);

        if (m_tail)
            m_tail->setNext(p);
        else
            m_head = p; // list was empty

        m_tail = p;
    }

    void push_clone(CPoint* src)
    {
        CPoint* np = new CPoint(src->time(), src->y(), src->pos());

        np->setCurve_m(src->curve_m());
        np->setCurve_n(src->curve_n());
        np->setEnbaled(src->enabled());
        np->setHovred(src->hovred());
        push_back(np);
    }

    // Insert keeping list sorted by time()
    void insert_sorted(CPoint* p)
    {
        if (!p)
            return;

        if (!m_head) {
            // empty list
            m_head = m_tail = p;
            p->setNext(nullptr);
            p->setPrev(nullptr);
            return;
        }

        if (p->time() <= m_head->time()) {
            push_front(p);
            return;
        }

        if (p->time() >= m_tail->time()) {
            push_back(p);
            return;
        }

        CPoint* cur = m_head;
        while (cur && cur->time() <= p->time())
            cur = cur->next();

        CPoint* prev = cur->prev();

        prev->setNext(p);
        p->setPrev(prev);

        p->setNext(cur);
        cur->setPrev(p);
    }

    // =====================================================================
    // Unlink / remove core
    // =====================================================================
    // Unlink from list but DO NOT delete
    void unlink(CPoint* p)
    {
        if (!p)
            return;

        if (p == m_head)
            m_head = p->next();
        if (p == m_tail)
            m_tail = p->prev();

        if (p->prev())
            p->prev()->setNext(p->next());
        if (p->next())
            p->next()->setPrev(p->prev());

        p->setNext(nullptr);
        p->setPrev(nullptr);
    }

    // Unlink and delete
    void remove(CPoint* p)
    {
        if (!p)
            return;
        unlink(p);
        delete p;
    }

    // =====================================================================
    // remove by time (first match; returns true if removed)
    // =====================================================================
    bool removeByTime(double t)
    {
        CPoint* cur = m_head;
        while (cur) {
            if (cur->time() == t) {
                remove(cur);
                return true;
            }
            cur = cur->next();
        }
        return false;
    }

    // =====================================================================
    // remove by CPoint* (returns true if it was found in list)
    // =====================================================================
    bool removeByPoint(CPoint* p)
    {
        if (!p)
            return false;

        // Optionally, we could check it's actually in the list.
        // For simplicity, we assume caller passes a valid node from this list.
        remove(p);
        return true;
    }

    // =====================================================================
    // Remove two points and everything between them.
    // Order does not matter: (left, right) or (right, left)
    // =====================================================================
    void removeBetween(CPoint* left, CPoint* right)
    {
        if (!left || !right || left == right)
            return;

        // Ensure left comes first in list
        CPoint* first = left;
        CPoint* second = right;

        // traverse forward to check correct order
        CPoint* cur = first;
        bool orderOK = false;

        while (cur) {
            if (cur == second) {
                orderOK = true;
                break;
            }
            cur = cur->next();
        }

        // if order was reversed, swap
        if (!orderOK) {
            std::swap(first, second);
        }

        // Now remove from first to second inclusive
        CPoint* before = first->prev();
        CPoint* after = second->next();

        cur = first;
        while (cur && cur != after) {
            CPoint* next = cur->next();
            unlink(cur); // detach safely
            delete cur; // delete node
            cur = next;
        }

        // Reconnect the list
        if (before)
            before->setNext(after);
        else
            m_head = after;

        if (after)
            after->setPrev(before);
        else
            m_tail = before;
    }

    // =====================================================================
    // insertBetween: insert p between left and right
    // assuming left and right are in this list and left comes before right
    // =====================================================================
    CPoint* insertBetween(CPoint* left, CPoint* right, CPoint* p)
    {
        if (!p)
            return nullptr;

        // If left or right is null, fall back to push_front/push_back.
        if (!left && !right) {
            // list empty or undefined, treat as push_back
            push_back(p);
            return p;
        }

        if (!left) {
            // insert before right
            return insertBefore(right, p);
        }

        if (!right) {
            // insert after left
            return insertAfter(left, p);
        }

        // normal: left <-> right
        p->setPrev(left);
        p->setNext(right);

        left->setNext(p);
        right->setPrev(p);

        if (m_tail == left)
            m_tail = p;

        if (m_head == right)
            m_head = p;

        return p;
    }

    void insertListAfter(CPoint* left, CPointList& other)
    {
        if (!other.m_head) {
            // other list is empty → nothing to do
            return;
        }

        // If THIS list is empty, just adopt the whole list
        if (!m_head) {
            m_head = other.m_head;
            m_tail = other.m_tail;

            // clear the source list
            other.m_head = other.m_tail = nullptr;
            return;
        }

        // CASE 1: insert at beginning (left == nullptr)
        if (!left) {
            CPoint* right = m_head;

            // adopt [other.head ... other.tail] before current head
            other.m_tail->setNext(right);
            right->setPrev(other.m_tail);

            m_head = other.m_head;

            other.m_head = other.m_tail = nullptr;
            return;
        }

        // CASE 2: insert after left normally
        CPoint* right = left->next();

        if (right) {
            // normal: left <-> other <-> right
            left->setNext(other.m_head);
            other.m_head->setPrev(left);

            other.m_tail->setNext(right);
            right->setPrev(other.m_tail);
        } else {
            // left was the tail → append at end
            left->setNext(other.m_head);
            other.m_head->setPrev(left);

            m_tail = other.m_tail;
        }

        // clear the source list
        other.m_head = other.m_tail = nullptr;
    }

    // =====================================================================
    // insertAfter / insertBefore
    // =====================================================================
    CPoint* insertAfter(CPoint* where, CPoint* p)
    {
        if (!p)
            return nullptr;

        if (!where) {
            // treat as push_front if no reference
            push_front(p);
            return p;
        }

        CPoint* nxt = where->next();

        p->setPrev(where);
        p->setNext(nxt);

        where->setNext(p);
        if (nxt)
            nxt->setPrev(p);
        else
            m_tail = p; // inserted at end

        return p;
    }

    CPoint* insertBefore(CPoint* where, CPoint* p)
    {
        if (!p)
            return nullptr;

        if (!where) {
            // treat as push_back if no reference
            push_back(p);
            return p;
        }

        CPoint* prv = where->prev();

        p->setNext(where);
        p->setPrev(prv);

        where->setPrev(p);
        if (prv)
            prv->setNext(p);
        else
            m_head = p; // inserted at beginning

        return p;
    }

    // =====================================================================
    // removeBefore / removeAfter
    // (remove & delete the node before/after 'where')
    // =====================================================================
    bool removeBefore(CPoint* where)
    {
        if (!where)
            return false;

        CPoint* prv = where->prev();
        if (!prv)
            return false;

        remove(prv);
        return true;
    }

    bool removeAfter(CPoint* where)
    {
        if (!where)
            return false;

        CPoint* nxt = where->next();
        if (!nxt)
            return false;

        remove(nxt);
        return true;
    }

    // =====================================================================
    // clear / empty / size
    // =====================================================================
    void clear()
    {
        CPoint* cur = m_head;
        while (cur) {
            CPoint* next = cur->next();
            delete cur;
            cur = next;
        }
        m_head = nullptr;
        m_tail = nullptr;
    }
    void print()
    {
        QString line;
        CPoint* cur = m_head;

        while (cur) {
            line += "( " + QString::number(cur->time()) + " " + QString::number(cur->y()) + " ) ";
            cur = cur->next();
        }

        qDebug() << line;
    }

    std::pair<double, double> getMinMaxY() const
    {
        if (!m_head) {
            return { 0.0, 0.0 }; // or throw, or use std::nullopt
        }

        const CPoint* cur = m_head;
        double minY = cur->y();
        double maxY = cur->y();

        cur = cur->next();
        while (cur) {
            double y = cur->y();
            if (y < minY)
                minY = y;
            if (y > maxY)
                maxY = y;
            cur = cur->next();
        }

        return { minY, maxY };
    }

    bool empty() const
    {
        return m_head == nullptr;
    }

    std::size_t size() const
    {
        std::size_t n = 0;
        for (CPoint* p = m_head; p; p = p->next())
            ++n;
        return n;
    }

    // =====================================================================
    // clone: deep-copy the list (time/y/pos only)
    // caller owns the returned pointer and must delete it
    // =====================================================================

    // =====================================================================
    // Iteration helpers
    // =====================================================================
    CPoint* head() const { return m_head; }
    CPoint* tail() const { return m_tail; }

    template <typename F>
    void for_each(F f) const
    {
        for (CPoint* p = m_head; p; p = p->next())
            f(p);
    }

    template <typename F>
    void for_each_reverse(F f) const
    {
        for (CPoint* p = m_tail; p; p = p->prev())
            f(p);
    }

private:
    CPoint* m_head = nullptr;
    CPoint* m_tail = nullptr;
};
// ====================================================================

class Automation : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool isAutomated READ isAutomated NOTIFY sigLinesChanged)
public:
    enum PointShape {
        PointShape_Circle = 0,
        PointShape_Dimond = 1,
    };

    std::vector<CLine> toQLinesScaleY(

        double xMin = -1000.0, // used when time is -∞
        double xMax = 1000.0 // used when time is +∞
    );

    std::vector<CLine> toSelectedLinesScale(

        double xMin = -1000.0, // used when time is -∞
        double xMax = 1000.0 // used when time is +∞
    );

    static int compareTime(std::shared_ptr<CPoint> a, std::shared_ptr<CPoint> b)
    {
        const bool a_inf = std::isinf(a->time());
        const bool b_inf = std::isinf(b->time());

        // both infinities of same sign → equal
        if (a_inf && b_inf) {
            if (std::signbit(a->time()) == std::signbit(b->time()))
                return 0;
            // -inf < +inf
            return std::signbit(a->time()) ? -1 : 1;
        }

        // handle infinities vs finite
        if (a_inf)
            return std::signbit(a->time()) ? -1 : 1; // -inf < finite < +inf
        if (b_inf)
            return std::signbit(b->time()) ? 1 : -1; // finite > -inf, < +inf

        // both finite → normal numeric compare
        if (a->time() < b->time())
            return -1;
        if (a->time() > b->time())
            return 1;
        return 0;
    }
    static bool lessTime(std::shared_ptr<CPoint> a, std::shared_ptr<CPoint> b)
    {
        return compareTime(a, b) < 0;
    }

    struct APoint {
        double time;
        double y;
        double curve_n = 1; // -10 << n << 10 n!=0
        double curve_m = 1; // -10 << n << 10 n!=0
    };

    std::mutex linesMutex;

    std::vector<CLine> getFinalLines()
    {

        return toQLinesScaleY(0, _areaInfo->lastWidth());
    }

    std::vector<CLine> getSelectedLines()
    {

        return toSelectedLinesScale(0, _areaInfo->lastWidth());
    }
    void setSelectedLineHovered(bool hovered)
    {
        _selectedLinesHovered = hovered;
        _hoveredLines.setAllHovered(hovered);
    }
    bool selectedLineHovered()
    {
        return _selectedLinesHovered;
    }
    void setLineHovered(CLine* index, bool hovered)
    {
        index->p1()->setHovred(hovered);
        index->p2()->setHovred(hovered);
    }
    void clearHovered()
    {
        _cLines.clearHovered();
    }

    QPointF dashedLineHovred()
    {
        return _dashedLineHovred;
    }

    void setDashedLineHovred(QPointF dashedLineHovred)
    {
        _dashedLineHovred = dashedLineHovred;
    }

    QPointF _dashedLineHovred;

    enum HitState {
        None,
        OnPoint, // near start OR end
        OnLine,
        NearLine
    };
    struct HitResult {
        CLine index;

        HitState state = HitState::None;
        int endpoint = -1; // 0=start, 1=end, -1=not an endpoint
        QPointF mapped = QPointF(-1, -1);

        void debug() const
        {
            qDebug() << "HitResult - index:"
                     << "state:" << static_cast<int>(state)
                     << "endpoint:" << endpoint << " pont" << mapped;
        }
    };

    const CPointList* selectedLine() const { return m_selected_line; }
    const CPointList* selectedLines() const { return _selectedLines; }

    CPointList* m_selected_line = nullptr;
    CPoint* m_selected_point_index = nullptr;

    CPointList* selectedLines()
    {
        return _selectedLines;
    }

    CPointList* selectedLine()
    {
        return m_selected_line;
    }
    CPointList* hoveredLines()
    {
        return &_hoveredLines;
    }

    void setSelectedLine(CPoint* start, CPoint* end);

    CPoint* selectedPointIndex()
    {
        return m_selected_point_index;
    }
    void setSelectedPointIndex(CPoint* index)
    {
        m_selected_point_index = index;
    }
    QPointF cPoint2QPoint(CPoint* p)
    {
        return QPointF(time2Pixel(p->time()), getYPixel(p->y()));
    }
    void clearHoveredLines();
    void createHoveredLines(double start_time, double end_time);
    void deleteLines(double start_time, double end_time);

    void insertLines(double start_time, double end_time, double indexSub);

    bool createCopyLines(double start_time, double end_time);

    void createSelectedLines();
    int hoveredLinesCount()
    {
        return _hoveredLines.size();
    }

    double getHoveredLinesHead()
    {
        if (_hoveredLines.size() > 0)
            return _hoveredLines.head()->time();

        return 0;
    }

    HitResult hitTestLines(
        double mouseX, double mouseY,
        double onLineThreshold = 3.0,
        double nearLineThreshold = 10.0,
        double pointRadius = 6.0);

    struct ALine {
        double startTime;
        double endTime;
        double startY;
        double endY;
        double curve_n = 1; // -10 << n << 10 n!=0
        double curve_m = 1; // -10 << n << 10 n!=0
        float tempo_start = 120;
        float tempo_end = 120;
        double _slope = 1;
        double _distance = 0;
        int _sign = 1;
        float rStartTime = 1;
        float rEndTime = 1;
        float height = 1;
        float yOffset = 0;
        float baseStartY = 0;
        float baseEndY = 0;
        float baseStartTime = 0;
        float baseEndTime = 0;
        bool reversed = false;
        PointShape _point_shape_start = PointShape::PointShape_Circle;
        PointShape _point_shape_end = PointShape::PointShape_Circle;

        void setState(QJsonObject _state)
        {
            reversed = _state["reversed"].toBool();
            baseEndTime = _state["baseEndTime"].toDouble();
            baseStartTime = _state["baseStartTime"].toDouble();
            baseEndY = _state["baseEndY"].toDouble();
            baseStartY = _state["baseStartY"].toDouble();
            yOffset = _state["yOffset"].toDouble();
            height = _state["height"].toDouble();
            rEndTime = _state["rEndTime"].toDouble();
            rStartTime = _state["rStartTime"].toDouble();
            _sign = _state["_sign"].toInt();
            _distance = _state["_distance"].toDouble();

            _slope = _state["_slope"].toDouble();
            tempo_end = _state["tempo_end"].toDouble();
            tempo_start = _state["tempo_start"].toDouble();
            curve_m = _state["curve_m"].toDouble();
            curve_n = _state["curve_n"].toDouble();
            endY = _state["endY"].toDouble();
            startY = _state["startY"].toDouble();
            endTime = _state["endTime"].toDouble();
            startTime = _state["startTime"].toDouble();
        }

        QJsonObject state()
        {
            QJsonObject _state;
            _state["startTime"] = startTime;
            _state["endTime"] = endTime;
            _state["startY"] = startY;
            _state["endY"] = endY;
            _state["curve_n"] = curve_n;
            _state["curve_m"] = curve_m;
            _state["tempo_start"] = tempo_start;
            _state["tempo_end"] = tempo_end;
            _state["_slope"] = _slope;

            _state["_distance"] = _distance;
            _state["_sign"] = _sign;
            _state["rStartTime"] = rStartTime;
            _state["rEndTime"] = rEndTime;
            _state["height"] = height;

            _state["yOffset"] = yOffset;
            _state["baseStartY"] = baseStartY;
            _state["baseEndY"] = baseEndY;
            _state["baseStartTime"] = baseStartTime;
            _state["baseEndTime"] = baseEndTime;
            _state["reversed"] = reversed;

            return _state;
        }
    };

    struct PolarPosition {
        double distance;
        double slope;
        int sign;
    };

    struct LineInfo {
        double slope; // 0, +inf, -inf, or normal
        double distance;
    };

    QPointF pointAfterDistance(const QPointF& start, double slope, double distance)
    {
        double dx = 0.0;
        double dy = 0.0;

        if (std::isinf(slope)) {
            // Vertical line
            if (slope > 0)
                dy = distance; // +∞ → upward
            else
                dy = -distance; // -∞ → downward
        } else if (slope == 0.0) {
            // Horizontal line
            dx = distance; // move right (you can customize direction if needed)
        } else {
            // Normal slope: convert slope to angle
            double theta = std::atan(slope);
            dx = distance * std::cos(theta);
            dy = distance * std::sin(theta);
        }

        return QPointF(start.x() + dx, start.y() + dy);
    }

    LineInfo getSlopeAndDistance(const QPointF& p1, const QPointF& p2)
    {
        double dx = p2.x() - p1.x();
        double dy = p2.y() - p1.y();

        LineInfo info;

        // Distance
        info.distance = std::sqrt(dx * dx + dy * dy);

        // Slope
        if (dx == 0.0) {
            // Vertical line → infinite slopes
            if (dy > 0)
                info.slope = std::numeric_limits<double>::infinity(); // +∞
            else if (dy < 0)
                info.slope = -std::numeric_limits<double>::infinity(); // -∞
            else
                info.slope = 0.0; // same point
        } else {
            info.slope = dy / dx;
        }

        return info;
    }

    explicit Automation(AreaInfo* areaInfo, int trackIndex, QString type, QString subType, float minY = 0, float maxY = 1, float defaultY = 0, QObject* parent = Q_NULLPTR);
    ~Automation();

    static bool nearlyEqual(double a, double b, double eps = 1e-9)
    {
        return std::abs(a - b) <= eps;
    }

    static bool validY(double y)
    {
        return y >= 0.0 && y <= 1.0;
    }

    std::vector<CLine>
    find_line_index(const CPointList* in, double t, double y = -1, double eps = 1e-9);

    void edit_point(CPoint* point, double t, double y, double eps = 1e-9);
    static inline bool lessStrict(double a, double b, double eps = 1e-9)
    {
        // Enforce strict increase: a < b (allowing tiny FP noise)
        return (b - a) > eps;
    }

    void edit_line(CPointList* list, QPointF newPoint, double eps = 1e-9);

    void delete_point(CPoint* point, double eps = 1e-9)
    {

        if (_cLines.empty())
            return;

        _cLines.remove(point);

        if (_cLines.size() == 2) {
            _cLines.head()->setY(_normalized);
            _cLines.tail()->setY(_normalized);
        }

        if (_cLines.tail()->isPosInf()) {
            if (_cLines.tail() && _cLines.tail()->prev()) {
                _cLines.tail()->setY(_cLines.tail()->prev()->y());
            }
        }

        if (_cLines.head()->isNegInf()) {
            if (_cLines.head() && _cLines.head()->next()) {
                _cLines.head()->setY(_cLines.head()->next()->y());
            }
        }
        Q_EMIT sigLinesChanged();
    }

    CPoint* add_point(double t, double y, double eps = 1e-9);

    void saveTemp()
    {
        std::lock_guard<std::mutex> lock(linesMutex);

        if (_selectedLines)
            _selectedLines->clear();

        if (m_selected_line) {
            m_selected_line->clear();
        }
        _hoveredLines.clear();

        if (_tLines.size() > 0) {
            _cLines = _tLines.clone();
            _tLines.clear();
        }

        if (_cLines.size() == 0) {
            _cLines.push_back(CPoint::NegInf(_normalized));
            _cLines.push_back(CPoint::PosInf(_normalized));
        }

        //_cLines.print();
    }
    void createTemp()
    {
        std::lock_guard<std::mutex> lock(linesMutex);
        // _tLines = _cLines.clone();
    }
    void clearTemp()
    {
        std::lock_guard<std::mutex> lock(linesMutex);
        //_tLines.clear();
    }

    bool _selectedLinesHovered = false;

    CPointList _tLines;
    CPointList _cLines;

    // CPointList _hoveredLines;
    CPointList _hoveredLines;
    CPointList* _selectedLines = nullptr;

    CPointList _copiedLines;

    CPoint _cPoint;

    void update(QRect windowRect);

    ALine getLine(int index);
    double getY(double time);

    bool contains(SelectedArea selectedArea);

    void selectAll();
    void clear();

    bool isAutomated();

    QVector<QLineF> list2Ranegs(bool moving);
    virtual QVector<QLineF> getRanges(bool _mouseMoving);
    QVector<QLineF> selectedLines2Ranegs();

    void insertLine(int index, QJsonObject state);
    void editLine(int index, QJsonObject state);

    void deleteLine(int index);

    QJsonArray getLinesState();
    void setLinesState(QJsonArray state);

    void saveTemplateLine();

    int find_line(double time);
    int find_line(float startx, float endx);
    int find_line(float SpixelX, float SpixelY, float EpixelX, float EpixelY);
    int find_line_start_point(float pixelX, float pixelY);

    int find_selected_line_start_point(float pixelX, float pixelY);

    int findSelected_point(float SpixelX, float SpixelY, float EpixelX, float EpixelY);

    bool getSelectedLinesCurve(int rangeIndex, float& n, float& m);

    void createSnapLine(bool pointIncluded);
    void createSnapLine(bool pointIncluded, QPointF hitPoint);
    void createSnapLine(bool pointIncluded, int excludePoint1, int excludePoint2);
    void findSnap(double posX, double posY, double& final_xpos, double& final_ypos);
    void findSnapWithThreshold(double posX,
        double posY,
        double& final_xpos,
        double& final_ypos,
        double threshold);
    virtual double time2Pixel(double time);
    virtual double pixel2Time(double pixel);
    virtual double time2Pixel_warpLess(double time);
    virtual double pixel2Time_warpLess(double pixel);
    virtual double width2Time(double time, double width);
    double pixel2Y(double Ypixel)
    {
        return (Ypixel - _windowRect.top()) / (double)(_windowRect.bottom() - _windowRect.top());
    }

    int findRangeIndex(float SposX, float SposY, float EposX, float EposY, bool moving = false);
    int findRangeIndex(float posX, float posY);
    int findRangeIndex(float posX);

    void setAutomationStartTempo(int index, float value);

    void createSelectedPoints(double startMarker, double endMarker);
    QVector<ALine> createSelectedLines(double startMarker, double endMarker);

    int getMaxYinSelectedPoints();
    int getMinYinSelectedPoints();

    bool isSelectedRange(int rangeIndex);

    void streachSelectedLines(double startTime, double duration, float down, float height, bool leftSide, bool rightSide, bool reverseUpDpwn, bool reverseLeftRight);
    void reverseSelectedLines();
    void moveSelectedPoints(QPointF currentPos);
    void clearSelectedLines();

    int find_first_selected_lines_index(double time);
    int find_first_first_list_index(double time);
    int find_last_selected_lines_index(double time);
    int find_last_last_list_index(double time);

    float find_first_selected_lines_Y(QVector<ALine> selected_lines, int index);
    float find_last_selected_lines_Y(QVector<ALine> selected_lines, int index);
    float find_first_first_lines_Y(int index);
    float find_last_last_lines_Y(int index);
    float find_lines_Y(QVector<ALine> lines, double time, int index);

    bool getSelectedRangeMoveEnabled() const;
    void setSelectedRangeMoveEnabled(bool selectedRangeMoveEnabled);

    bool findNewRoot(int _currentRangeIndex, double time, float& p, float& q, float& r, float& s, float& y);
    bool findNewRoot(QVector<ALine> lines, int index, double time, float& p, float& q, float& r, float& s, float& y);

    void clearMovingLine();

    QVector<ALine> cloneLines();

    void setSelectedLines(QVector<Automation::ALine> lines, int startIndex, int endIndex);
    QVector<Automation::ALine> cloneSelectedLines();

    bool getStopSteachUp() const;
    void setStopSteachUp(bool stopSteachUp);

    bool getLimitUpStreach() const;
    void setLimitUpStreach(bool limitUpStreach);

    bool getLimitDownStreach() const;
    void setLimitDownStreach(bool limitDownStreach);

    QVector<ALine> streachUpChecking();
    QVector<ALine> streachDownChecking(QVector<ALine> _selected_lines_temp_temp);

    bool isZero(double p1, double p2)
    {
        if (qAbs(p1 - p2) < 0.0001) {
            return true;
        }
        return false;
    }

    double getFirstPoint();
    QPointF getFirstSelectedPoint();
    QPointF getLastSelectedPoint();
    double getLastPoint();

    int getSelectedStartIndex() const;

    int getSelectedEndIndex() const;

    void setSelectedStartIndex(int selectedStartIndex);

    void setSelectedEndIndex(int selectedEndIndex);

    Automation::ALine getStartPoint();
    void setStartPoint(const ALine& startPoint);

    Automation::ALine getEndPoint();
    void setEndPoint(const ALine& endPoint);

    Automation::ALine getFirst_point();
    void setFirst_point(const ALine& first_point);

    Automation::ALine getLast_point();
    void setLast_point(const ALine& last_point);

    QVector<Automation::ALine> getFirst_list();
    void setFirst_list(const QVector<ALine>& first_list);

    QVector<Automation::ALine> getLast_list();
    void setLast_list(const QVector<ALine>& last_list);

    bool getInit_selected_line_move() const;
    void setInit_selected_line_move(bool init_selected_line_move);

    QJsonArray copyList() const;
    void setCopyList(const QJsonArray copyList);
    void clearCopyList();

    QRect getWindowRect() const;

    double getStartMarker() const;

    double getEndMarker() const;

    int getTopMarker() const;

    int getBottomMarker() const;

    bool enabled() const;
    void setEnabled1(bool enabled);

    int getSubTrackIndex() const;
    void setSubTrackIndex(int subTrackIndex);

    bool getAsyncClear() const;

    double automatedNormalized() const;
    void setAutomatedNormalized(double newAutomatedNormalized);

    double defaultNormalized() const;
    void setDefaultNormalized(double newDefaultNormalized);

    double normalized() const;
    void setNormalized(double newNormalized);

    double minValue() const;
    void setMinValue(double newMinValue);

    double maxValue() const;
    void setMaxValue(double newMaxValue);

    QString unit() const;
    void setUnit(const QString& newUnit);

    int trackIndex() const;

    QString type() const;
    QString subType() const;

    bool discreted() const;
    void setDiscreted(bool newDiscreted);

    double getYPixel(double y);

    float minY() const;

    float maxY() const;

    bool fixedVertical() const;
    void setFixedVertical(bool newFixedVertical);

    PointShape pointShape() const;
    void setPointShape(PointShape newPointShape);

    void encofrMonotone(std::vector<std::pair<CPoint*, CPoint*>>& lines);

protected:
    bool _discreted = false;
    // This is for KeyFramesAutomationItem
    bool _fixedVertical = false;

    int _discretedCount = 2;

    float m_minY = 0;
    float m_maxY = 1;
    QString _type;
    QString _subType;
    QList<Warp> warps;
    bool _selectAll = false;
    bool _asyncClear = false;
    QVector<QLineF> _ranges;
    QVector<ALine> _lines;
    QVector<ALine> _temp_lines;
    QVector<ALine> _selected_lines_temp;
    QVector<ALine> _selected_lines;
    typedef QPair<double, double> Snap;
    QVector<Snap> _lastSnapAutomationPoints;
    QVector<Snap> _snapPoints;

    QVector<int> _selected_for_moving_line;
    QVector<int> _selected_point_index;
    QVector<ALine> _selected_point;

    QRect _windowRect;

    QVector<PolarPosition> _selected_line_positions;

    double selectedTimeDuration;
    double selectedTimeStart;

    bool _init_selected_line_move = false;
    bool _init_selected_point_move = true;
    int _selectedStartIndex = -1;
    int _selectedEndIndex = -1;

    ALine _start_line;
    ALine _end_line;
    ALine _first_line;
    ALine _last_line;
    QVector<ALine> _first_list;
    QVector<ALine> _last_list;

    QVector<APoint> _selected_point2;
    bool _selectedRangeMoveEnabled = false;
    float _start_point_y;
    float _end_point_y;

    int _start_index;
    int _end_index;

    double _base_start_time;

    bool _limitUpStreach;
    bool _limitDownStreach;

    double _startMarker;
    double _endMarker;
    int _topMarker;
    int _bottomMarker;

    // All Values are normalized between 0~1
    double _automatedNormalized;
    double _defaultNormalized;
    double _normalized = 0.25;

    double _minValue;
    double _maxValue;

    QString _unit;

    bool _enabled = false;

    int _trackIndex = -1;
    int _subTrackIndex = -1;

    PointShape _pointShape = PointShape::PointShape_Circle;

protected:
    AreaInfo* _areaInfo;

private:
    double getStartY(double time, int index);
    double getStartTime(float y, QVector<ALine> selected_lines, int index);

signals:
    void sigStartMarkerChanged(double stratMarker);
    void sigEndMarkerChanged(double endMarker);

    void sigMaxEndTime(double time);
    void sigMinStartTime(double time);

    void valueChanged();

    void sigLinesChanged();
    void enabledChanged();

    void sigUpdate();

    void sigAddLine(QLine line);
};

#endif // AUTOMATION_H
