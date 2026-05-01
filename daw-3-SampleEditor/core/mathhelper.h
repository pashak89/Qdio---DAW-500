#ifndef MATHHELPER_H
#define MATHHELPER_H
#include <QDebug>
#include <QLine>
#include <QMap>
#include <QPair>
#include <QPointF>
#include <QPolygonF>
#include <QtMath>
#include <xmmintrin.h>

class MathHelper1 {
public:
    static double pan2leftGain(double pan)
    {
        double scaled = pan * M_PI_2 - M_PI_4;
        return (cos(scaled) - sin(scaled)) * sqrt(2.0) / 2.0;
    }
    static double pan2RightGain(double pan)
    {
        double scaled = pan * M_PI_2 - M_PI_4;
        return (cos(scaled) + sin(scaled)) * sqrt(2.0) / 2.0;
    }

    static double yPixel2Decibel(double Y)
    {
        double X = -42.3529 * pow(Y, 2) - 33.6471 * Y + 6;
        return X;
    }

    static double decibel2yPixel(double X)
    {
        double a = -42.3529;
        double b = -33.6471;
        double c = 6;

        double discriminant = pow(b, 2) - 4 * a * (c - X);
        double sqrtDiscriminant = sqrt(discriminant);

        // Check if the roots are real
        if (discriminant >= 0) {
            double root1 = (-b + sqrtDiscriminant) / (2 * a);
            double root2 = (-b - sqrtDiscriminant) / (2 * a);
            // Return the positive root (assuming a quadratic has two real roots)
            return (root1 >= 0) ? root1 : root2;
        } else {
            // If discriminant < 0, no real roots exist
            return NAN; // Return NaN (Not-a-Number) to indicate no real roots
        }
    }

    static void findRoot(float p, float q, float x_center, float y_center, float x_radius, float y_radius, float sample_number, float x_point, float y_point, float& P, float& Q, float& R, float& S)
    {

        float a = x_radius;
        float b = y_radius;

        float xc = x_center;
        float yc = y_center;
        float xm = x_point;
        float ym = y_point;

        QVector<float> X;
        QVector<float> Y;
        ecllips_main(p, q, a, b, xc, yc, xm, ym, sample_number, X, Y);
        float J_min = qInf();
        QVector<float> P1;
        QVector<float> P2;
        float r = ((p + q) / 2.0) / 2.0;

        for (int i = 0; i < 50; i++) {

            QVector<float> X1;
            QVector<float> Y1;
            QVector<float> X2;
            QVector<float> Y2;
            QVector<float> R1;
            QVector<float> R2;
            ecllips_sub(X, p, q, r, a, b, xc, yc, xm, ym, X1, Y1, X2, Y2, R1, R2);

            QVector<float> Yt;
            Yt.append(Y1);
            Yt.append(Y2);

            auto sum = [=](QVector<float> x) { float sum = 0; for(int i=0;i<x.size();i++){ sum+=x[i]; } return sum; };
            auto abss = [=](QVector<float> x) { QVector<float> out; for(int i=0;i<x.size();i++){ out.append(x[i]); } return out; };
            float e = (sum(Y) - sum(Yt)) / (double)sum(abss(Y));

            auto minus = [=](QVector<float> x, QVector<float> y) { QVector<float> out; for(int i=0;i<y.size();i++){ out.append(x[i]-y[i]); } return out; };
            auto powes = [=](QVector<float> x) { QVector<float> out; for(int i=0;i<x.size();i++){ out.append(qPow(x[i],2)); } return out; };
            auto mean = [=](QVector<float> x) { float sum = 0; for(int i=0;i<x.size();i++){ sum+=x[i]; } return sum/(float)x.size(); };

            float J = sqrt(mean(powes(minus(Y, Yt))) / (float)mean(powes(Y))) * 100;

            if (J < J_min) {
                P1 = R1;
                P2 = R2;
                J_min = J;
            }

            r = r + e * sign(ym - yc);
        }

        //% first eclips parameters:
        //% x - order
        P = P1[4];
        //% y - order
        Q = P1[5];
        // second eclips parameters:
        // % x - order
        R = P2[4];
        //  % y - order
        S = P2[5];
    }
    static QVector<float> linspace(double x1, float x2, int n)
    {
        QVector<float> vector;
        float step = (x2 - x1) / (float)(n - 1);
        float start = x1;
        for (int i = 0; i < n; i++) {
            vector.append(start);
            start += step;
        }
        return vector;
    }
    static int sign(float x)
    {
        if (x < 0) {
            return -1;
        } else {
            return 1;
        }
    }
    static void ecllips_main(float p, float q, float a, float b, float xc, float yc, float xm, float ym, int step, QVector<float>& x, QVector<float>& y)
    {

        if (xm > xc) {
            x = linspace(xc, xc + a, step);
        } else {
            x = linspace(xc - a, xc, step);
        }

        for (int i = 0; i < x.size(); i++) {
            y.append(yc + sign(ym - yc) * b * qPow(1 - qPow(abs((x[i] - xc) * (1.0 / a)), p), (1.0 / q)));
            if (isnan(y[i]))
                y[i] = 0;
        }
    }

    static void ecllips_sub(QVector<float> xin, float p, float q, float r, float a, float b, float xc, float yc, float xm, float ym, QVector<float>& x1, QVector<float>& y1,
        QVector<float>& x2, QVector<float>& y2, QVector<float>& P1, QVector<float>& P2)
    {

        float p1, q1, p2, q2, a1, a2, b1, b2, xc1, yc1, xc2, yc2;
        if (xm > xc && ym > yc) {

            auto f1 = [=](QVector<float> xin, float xc, float xm) {QVector<float> out; for(int i=0;i<xin.size();i++){ if(xin[i]>=xc && xin[i] <xm){out.append(xin[i]);}} return out; };
            auto f2 = [=](QVector<float> xin, float xm, float xc, float a) {QVector<float> out; for(int i=0;i<xin.size();i++){ if(xin[i]>=xm && xin[i] <=(xc+a+0.1)){out.append(xin[i]);}} return out; };
            x1 = f1(xin, xc, xm);
            x2 = f2(xin, xm, xc, a);
            p1 = p;
            q1 = r;
            p2 = r;
            q2 = q;
            a1 = abs(xm - xc);
            b1 = b - abs(ym - yc);
            a2 = a - abs(xm - xc);
            b2 = abs(ym - yc);
            xc1 = xc;
            yc1 = ym;
            xc2 = xm;
            yc2 = yc;
        } else if (xm > xc && ym < yc) {

            auto f1 = [=](QVector<float> xin) {QVector<float> out; for(int i=0;i<xin.size();i++){ if(xin[i]>=xc && xin[i] <xm){out.append(xin[i]);}} return out; };
            auto f2 = [=](QVector<float> xin) {QVector<float> out; for(int i=0;i<xin.size();i++){ if(xin[i]>=xm && xin[i] <=xc+a){out.append(xin[i]);}} return out; };
            x1 = f1(xin);
            x2 = f2(xin);
            p1 = p;
            q1 = r;
            p2 = r;
            q2 = q;
            a1 = abs(xm - xc);
            b1 = b - abs(ym - yc);
            a2 = a - abs(xm - xc);
            b2 = abs(ym - yc);
            xc1 = xc;
            yc1 = ym;
            xc2 = xm;
            yc2 = yc;
        } else if (xm < xc && ym > yc) {

            auto f1 = [=](QVector<float> xin) {QVector<float> out; for(int i=0;i<xin.size();i++){ if(xin[i]>=xc-a && xin[i] <xm){out.append(xin[i]);}} return out; };
            auto f2 = [=](QVector<float> xin) {QVector<float> out; for(int i=0;i<xin.size();i++){ if(xin[i]>=xm && xin[i] <=xc){out.append(xin[i]);}} return out; };
            x1 = f1(xin);
            x2 = f2(xin);
            p1 = r;
            q1 = q;
            p2 = p;
            q2 = r;
            a1 = a - abs(xm - xc);
            b1 = abs(ym - yc);
            a2 = abs(xm - xc);
            b2 = b - abs(ym - yc);
            xc1 = xm;
            yc1 = yc;
            xc2 = xc;
            yc2 = ym;
        } else if (xm < xc && ym < yc) {
            auto f1 = [=](QVector<float> xin) {QVector<float> out; for(int i=0;i<xin.size();i++){ if(xin[i]>=xc-a && xin[i] <xm){out.append(xin[i]);}} return out; };
            auto f2 = [=](QVector<float> xin) {QVector<float> out; for(int i=0;i<xin.size();i++){ if(xin[i]>=xm && xin[i] <=xc){out.append(xin[i]);}} return out; };
            x1 = f1(xin);
            x2 = f2(xin);
            p1 = r;
            q1 = q;
            p2 = p;
            q2 = r;
            a1 = a - abs(xm - xc);
            b1 = abs(ym - yc);
            a2 = abs(xm - xc);
            b2 = b - abs(ym - yc);
            xc1 = xm;
            yc1 = yc;
            xc2 = xc;
            yc2 = ym;
        }

        for (int i = 0; i < x1.size(); i++) {
            y1.append(yc1 + sign(ym - yc) * b1 * qPow(1 - qPow(abs((x1[i] - xc1) * (1.0 / a1)), p1), (1.0 / q1)));
            if (isnan(y1[i]))
                y1[i] = 0;
        }
        for (int i = 0; i < x2.size(); i++) {
            y2.append(yc2 + sign(ym - yc) * b2 * qPow(1 - qPow(abs((x2[i] - xc2) * (1.0 / a2)), p2), (1.0 / q2)));
            if (isnan(y2[i]))
                y2[i] = 0;
        }
        P1.append(a1);
        P1.append(b1);
        P1.append(xc1);
        P1.append(yc1);
        P1.append(p1);
        P1.append(q1);

        P2.append(a2);
        P2.append(b2);
        P2.append(xc2);
        P2.append(yc2);
        P2.append(p2);
        P2.append(q2);
    }
    static double slope(QPointF p1, QPointF p2)
    {
        if (p1.x() == -qInf()) {
            p1.setX(p2.x());
        }

        if (p2.x() == qInf()) {
            p2.setX(p1.x());
        }

        double a;
        if (qAbs(p2.x() - p1.x()) > 0) {
            double m = (p2.y() - p1.y()) / (p2.x() - p1.x());
            a = qRadiansToDegrees(qAtan(m));
        } else {
            a = qInf();
        }
        return a;
    }
    static double function(double a, double n, double t)
    {
        return qPow(1 - qPow((t / a), n), 1 / n);
    }
    static QPointF mapPointToCurve(QPointF start, QPointF end, QPointF point, float n, float m)
    {

        double s = computeSlopeMagnitude(end, start, m, m,
            point.x(), point.y());

        QPointF result;
        if (s < 1) {
            result = MathHelper1::superEllipseXtoY(end, start, n, m,
                point.x());
        } else {
            result = MathHelper1::superEllipseYtoX(end, start, n, m,
                point.y());
        }
        return result;
    }

    static QPointF mapPointToLine(QPointF start, QPointF end, QPointF point)
    {
        if (qAbs((end.x() - start.x())) > 0) {
            double m = (end.y() - start.y()) / (end.x() - start.x());
            double a = qRadiansToDegrees(qAtan(m));
            double Y = m * (point.x() - start.x()) + start.y();
            double H = point.y() - Y;

            double N = H * qCos(qDegreesToRadians(90 - a)) * qCos(qDegreesToRadians(a));

            double newX = point.x() + N;

            double newY = m * (newX - start.x()) + start.y();

            return QPointF(newX, newY);
        } else {

            return QPointF(start.x(), point.y());
        }
    }
    static QPolygonF superEllipseArea(QPointF a, QPointF b, double n, double m, double width)
    {
        QPolygonF poly;
        if (n < 0 || m < 0) {
            n = qAbs(n);
            m = qAbs(m);

            int centerX = a.x();
            int centerY = b.y();

            QPoint bb = QPoint(b.x() + 20, b.y());
            QPoint aa = QPoint(a.x(), a.y() - 20);

            double A = aa.y() - centerY;
            double B = centerX - bb.x();

            poly.append(bb);

            double sp = bb.x();
            double ep = centerX;
            if (sp < 0)
                sp = 0;
            if (ep > width) {
                ep = width;
            }

            for (int x = sp; x <= ep; x++) {

                double y = A * qPow(1 - qPow(qAbs((x - centerX) / (double)B), n), 1.0 / (double)m) + centerY; // solve for y

                poly.append(QPointF(x, y));
            }
            {
                int centerX = a.x();
                int centerY = b.y();

                QPoint bb = QPoint(b.x() - 20, b.y());
                QPoint aa = QPoint(a.x(), a.y() + 20);

                double A = aa.y() - centerY;
                double B = centerX - bb.x();

                poly.append(aa);

                double sp = centerX;
                double ep = bb.x();
                if (sp < 0)
                    sp = 0;
                if (ep > width) {
                    ep = width;
                }

                for (int x = sp; x >= ep; x--) {

                    double y = A * qPow(1 - qPow(qAbs((x - centerX) / (double)B), n), 1.0 / (double)m) + centerY; // solve for y

                    poly.append(QPointF(x, y));
                }
            }
            poly.append(QPointF(b.x() + 20, b.y()));

        } else {
            n = qAbs(n);

            int centerX = b.x();
            int centerY = a.y();

            QPoint bb = QPoint(b.x(), b.y() - 20);
            QPoint aa = QPoint(a.x() + 20, a.y());

            double A = aa.x() - centerX;
            double B = centerY - bb.y();

            poly.append(aa);

            double sp = aa.x();
            double ep = centerX;
            if (sp > width)
                sp = width;
            if (ep < 0) {
                ep = 0;
            }
            for (int x = sp; x >= ep; x--) {

                double y = -B * qPow(1 - qPow(qAbs((x - centerX) / (double)A), n), 1.0 / (double)m) + centerY; // solve for y
                poly.append(QPointF(x, y));
            }

            {
                int centerX = b.x();
                int centerY = a.y();

                QPoint bb = QPoint(b.x(), b.y() + 20);
                QPoint aa = QPoint(a.x() - 20, a.y());

                double A = aa.x() - centerX;
                double B = centerY - bb.y();

                poly.append(bb);

                double sp = centerX;
                double ep = bb.x();
                if (sp < 0)
                    sp = 0;
                if (ep > width) {
                    ep = width;
                }

                for (int x = sp; x <= ep; x++) {

                    double y = -B * qPow(1 - qPow(qAbs((x - centerX) / (double)A), n), 1.0 / (double)m) + centerY; // solve for y
                    poly.append(QPointF(x, y));
                }
            }
            poly.append(QPoint(a.x() + 20, a.y()));
        }

        return poly;
    }
    static bool superEllipseContains(QPointF a, QPointF b, double n, double m, double x, double y)
    {
        if (n < 0 || m < 0) {
            n = qAbs(n);
            m = qAbs(m);
            int centerX = a.x();
            int centerY = b.y();

            QPoint bb = QPoint(b.x(), b.y());
            QPoint aa = QPoint(a.x(), a.y());

            double A = aa.y() - centerY;
            double B = centerX - bb.x();

            if (qAbs(qPow(qAbs((x - centerX) / B), n) + qPow(qAbs((y - centerY) / A), m) - 1) < 0.1) {

                return true;
            }
        } else {
            n = qAbs(n);
            int centerX = b.x();
            int centerY = a.y();

            QPoint bb = QPoint(b.x(), b.y());
            QPoint aa = QPoint(a.x(), a.y());

            double A = aa.x() - centerX;
            double B = centerY - bb.y();

            if (qAbs(qPow(qAbs((x - centerX) / A), n) + qPow(qAbs((y - centerY) / B), m) - 1) < 0.1) {

                return true;
            }
        }

        return false;
    }
    static bool isInSuperEllipseArea(QPointF a, QPointF b, double n, double m, double x, double y, int outerThreshold)
    {
        if (n < 0 || m < 0) {
            n = qAbs(n);
            m = qAbs(m);

            int centerX = a.x();
            int centerY = b.y();

            if (a.y() < b.y()) {
                QPoint bb = QPoint(b.x() - outerThreshold, b.y());
                QPoint aa = QPoint(a.x(), a.y() - outerThreshold);

                double A = qAbs(bb.x() - centerX);
                double B = qAbs(centerY - aa.y());

                QPoint bb1 = QPoint(b.x() + outerThreshold, b.y());
                QPoint aa1 = QPoint(a.x(), a.y() + outerThreshold);

                double A1 = qAbs(bb1.x() - centerX);
                double B1 = qAbs(centerY - aa1.y());

                if (qPow(qAbs((x - centerX) / (double)A), n) + qPow((qAbs((y - centerY) / (double)B)), m) < 1) {
                    if (qPow(qAbs((x - centerX) / (double)A1), n) + qPow((qAbs((y - centerY) / (double)B1)), m) > 1) {

                        if (x - centerX < 0 && y - centerY < 0)
                            return true;
                        return false;
                    }
                }
            } else {

                QPoint bb = QPoint(b.x() + outerThreshold, b.y());
                QPoint aa = QPoint(a.x(), a.y() - outerThreshold);

                double A = qAbs(bb.x() - centerX);
                double B = qAbs(centerY - aa.y());

                QPoint bb1 = QPoint(b.x() - outerThreshold, b.y());
                QPoint aa1 = QPoint(a.x(), a.y() + outerThreshold);

                double A1 = qAbs(bb1.x() - centerX);
                double B1 = qAbs(centerY - aa1.y());

                if (qPow(qAbs((x - centerX) / (double)A), n) + qPow((qAbs((y - centerY) / (double)B)), m) > 1) {
                    if (qPow(qAbs((x - centerX) / (double)A1), n) + qPow((qAbs((y - centerY) / (double)B1)), m) < 1) {

                        if (x - centerX < 0 && y - centerY > 0)
                            return true;

                        return false;
                    }
                }
            }

        } else {
            n = qAbs(n);

            if (a.y() > b.y()) {
                int centerX = b.x();
                int centerY = a.y();
                QPoint bb = QPoint(b.x(), b.y() - outerThreshold);
                QPoint aa = QPoint(a.x() + outerThreshold, a.y());

                double A = aa.x() - centerX;
                double B = centerY - bb.y();

                QPoint bb1 = QPoint(b.x(), b.y() + outerThreshold);
                QPoint aa1 = QPoint(a.x() - outerThreshold, a.y());

                double A1 = aa1.x() - centerX;
                double B1 = centerY - bb1.y();

                if (qPow(qAbs((x - centerX) / (double)A), n) + qPow((qAbs((y - centerY) / (double)B)), m) < 1) {
                    if (qPow(qAbs((x - centerX) / (double)A1), n) + qPow((qAbs((y - centerY) / (double)B1)), m) > 1) {

                        if (x - centerX > 0 && y - centerY < 0)
                            return true;

                        return false;
                    }
                }
            } else {
                int centerX = b.x();
                int centerY = a.y();
                QPoint bb = QPoint(b.x(), b.y() - outerThreshold);
                QPoint aa = QPoint(a.x() - outerThreshold, a.y());

                double A = aa.x() - centerX;
                double B = centerY - bb.y();

                QPoint bb1 = QPoint(b.x(), b.y() + outerThreshold);
                QPoint aa1 = QPoint(a.x() + outerThreshold, a.y());

                double A1 = aa1.x() - centerX;
                double B1 = centerY - bb1.y();

                if (qPow(qAbs((x - centerX) / (double)A), n) + qPow((qAbs((y - centerY) / (double)B)), m) > 1) {
                    if (qPow(qAbs((x - centerX) / (double)A1), n) + qPow((qAbs((y - centerY) / (double)B1)), m) < 1) {

                        if (x - centerX > 0 && y - centerY > 0)
                            return true;

                        return false;
                    }
                }
            }
        }

        return false;
    }
    static double lineAreasDiscreted(QPointF p1, QPointF p2, QPolygonF& inner, QPolygonF& outer, int innerThreshold, int outerThreshold)
    {
        inner.clear();
        inner.append(QPointF(p1.x(), p1.y() - innerThreshold));
        inner.append(QPointF(p2.x(), p1.y() - innerThreshold));
        inner.append(QPointF(p2.x(), p1.y() + innerThreshold));
        inner.append(QPointF(p1.x(), p1.y() + innerThreshold));
        inner.append(QPointF(p1.x(), p1.y() - innerThreshold));

        if (p2.y() < p1.y()) {
            outer.clear();
            outer.append(QPointF(p1.x(), p1.y() - outerThreshold));
            outer.append(QPointF(p2.x() - outerThreshold, p1.y() - outerThreshold));
            outer.append(QPointF(p2.x() - outerThreshold, p2.y()));
            outer.append(QPointF(p2.x() + outerThreshold, p2.y()));
            outer.append(QPointF(p2.x() + outerThreshold, p1.y() + outerThreshold));
            outer.append(QPointF(p1.x(), p1.y() + outerThreshold));
            outer.append(QPointF(p1.x(), p1.y() - outerThreshold));

        } else {

            outer.clear();
            outer.append(QPointF(p1.x(), p1.y() - outerThreshold));
            outer.append(QPointF(p2.x() + outerThreshold, p1.y() - outerThreshold));
            outer.append(QPointF(p2.x() + outerThreshold, p2.y() + outerThreshold));

            outer.append(QPointF(p2.x() - outerThreshold, p2.y() + outerThreshold));
            outer.append(QPointF(p2.x() - outerThreshold, p1.y() + outerThreshold));
            outer.append(QPointF(p1.x(), p1.y() + outerThreshold));
            outer.append(QPointF(p1.x(), p1.y() - outerThreshold));
        }

        return 0;
    }

    static double lineAreas(QPointF p1, QPointF p2, QPolygonF& inner, QPolygonF& outer, int innerThreshold, int outerThreshold)
    {
        if (p1.x() == -qInf()) {
            p1.setX(0);
        }

        if (p2.x() == qInf()) {
            p2.setX(100000000);
        }

        if (qAbs(p2.x() - p1.x()) > 0) {
            double m = (p2.y() - p1.y()) / (p2.x() - p1.x());
            double a = qRadiansToDegrees(qAtan(m));
            double E = outerThreshold * qSin(qDegreesToRadians(90 - a));
            double D = outerThreshold * qCos(qDegreesToRadians(90 - a));

            double x0 = p1.x() + D;
            double y0 = p1.y() - E;
            double x1 = p1.x() - D;
            double y1 = p1.y() + E;

            double x2 = p2.x() - D;
            double y2 = p2.y() + E;
            double x3 = p2.x() + D;
            double y3 = p2.y() - E;

            outer.clear();
            outer.append(QPointF(x0, y0));
            outer.append(QPointF(x1, y1));
            outer.append(QPointF(x2, y2));
            outer.append(QPointF(x3, y3));
            outer.append(QPointF(x0, y0));

            E = innerThreshold * qSin(qDegreesToRadians(90 - a));
            D = innerThreshold * qCos(qDegreesToRadians(90 - a));

            x0 = p1.x() + D;
            y0 = p1.y() - E;
            x1 = p1.x() - D;
            y1 = p1.y() + E;

            x2 = p2.x() - D;
            y2 = p2.y() + E;
            x3 = p2.x() + D;
            y3 = p2.y() - E;

            inner.clear();
            inner.append(QPointF(x0, y0));
            inner.append(QPointF(x1, y1));
            inner.append(QPointF(x2, y2));
            inner.append(QPointF(x3, y3));
            inner.append(QPointF(x0, y0));
            return m;
        } else {

            double E = outerThreshold;
            double D = outerThreshold;

            double x0 = p1.x() - D;
            double y0 = p1.y();
            double x1 = p1.x() + D;
            double y1 = p1.y();

            double x2 = p2.x() + D;
            double y2 = p2.y();
            double x3 = p2.x() - D;
            double y3 = p2.y();

            outer.clear();
            outer.append(QPointF(x0, y0));
            outer.append(QPointF(x1, y1));
            outer.append(QPointF(x2, y2));
            outer.append(QPointF(x3, y3));
            outer.append(QPointF(x0, y0));

            E = innerThreshold;
            D = innerThreshold;

            x0 = p1.x() - D;
            y0 = p1.y();
            x1 = p1.x() + D;
            y1 = p1.y();

            x2 = p2.x() + D;
            y2 = p2.y();
            x3 = p2.x() - D;
            y3 = p2.y();

            inner.clear();
            inner.append(QPointF(x0, y0));
            inner.append(QPointF(x1, y1));
            inner.append(QPointF(x2, y2));
            inner.append(QPointF(x3, y3));
            inner.append(QPointF(x0, y0));
            return 0;
        }
    }

    static double lineAreas2(QPointF p1, QPointF p2, QPolygonF& inner, QPolygonF& outer)
    {
        if (p1.x() == -qInf()) {
            p1.setX(0);
        }

        if (p2.x() == qInf()) {
            p2.setX(100000000);
        }

        if (qAbs(p2.x() - p1.x()) > 0) {
            double m = (p2.y() - p1.y()) / (p2.x() - p1.x());
            double a = qRadiansToDegrees(qAtan(m));
            double E = 20 * qSin(qDegreesToRadians(90 - a));
            double D = 20 * qCos(qDegreesToRadians(90 - a));

            double x0 = p1.x() + D;
            double y0 = p1.y() - E;
            double x1 = p1.x() - D;
            double y1 = p1.y() + E;

            double x2 = p2.x() - D;
            double y2 = p2.y() + E;
            double x3 = p2.x() + D;
            double y3 = p2.y() - E;

            outer.clear();
            outer.append(QPointF(x0, y0));
            outer.append(QPointF(x1, y1));
            outer.append(QPointF(x2, y2));
            outer.append(QPointF(x3, y3));
            outer.append(QPointF(x0, y0));

            E = 0.1 * qSin(qDegreesToRadians(90 - a));
            D = 0.1 * qCos(qDegreesToRadians(90 - a));

            x0 = p1.x() + D;
            y0 = p1.y() - E;
            x1 = p1.x() - D;
            y1 = p1.y() + E;

            x2 = p2.x() - D;
            y2 = p2.y() + E;
            x3 = p2.x() + D;
            y3 = p2.y() - E;

            inner.clear();
            inner.append(QPointF(x0, y0));
            inner.append(QPointF(x1, y1));
            inner.append(QPointF(x2, y2));
            inner.append(QPointF(x3, y3));
            inner.append(QPointF(x0, y0));
            return m;
        } else {

            double E = 0.2;
            double D = 0.2;

            double x0 = p1.x() - D;
            double y0 = p1.y();
            double x1 = p1.x() + D;
            double y1 = p1.y();

            double x2 = p2.x() + D;
            double y2 = p2.y();
            double x3 = p2.x() - D;
            double y3 = p2.y();

            outer.clear();
            outer.append(QPointF(x0, y0));
            outer.append(QPointF(x1, y1));
            outer.append(QPointF(x2, y2));
            outer.append(QPointF(x3, y3));
            outer.append(QPointF(x0, y0));

            E = 0.1;
            D = 0.1;

            x0 = p1.x() - D;
            y0 = p1.y();
            x1 = p1.x() + D;
            y1 = p1.y();

            x2 = p2.x() + D;
            y2 = p2.y();
            x3 = p2.x() - D;
            y3 = p2.y();

            inner.clear();
            inner.append(QPointF(x0, y0));
            inner.append(QPointF(x1, y1));
            inner.append(QPointF(x2, y2));
            inner.append(QPointF(x3, y3));
            inner.append(QPointF(x0, y0));
            return 0;
        }
    }

    static bool isLeft(QPointF a, QPointF b, QPointF c)
    {
        return ((b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x())) > 0;
    }
    static double distance(QPointF a, QPointF b)
    {
        return qSqrt(qPow(b.y() - a.y(), 2) + qPow(b.x() - a.x(), 2));
    }
    static double distance(double x1, double x2, double y1, double y2)
    {
        return qSqrt(qPow(x2 - x1, 2) + qPow(y2 - y1, 2));
    }
    static QPair<double, double> pointToD(QPointF p1, QPointF p2, QPointF point)
    {
        if (p1.x() == -qInf()) {
            p1.setX(p2.x());
        }

        if (p2.x() == qInf()) {
            p2.setX(p1.x());
        }

        if (qAbs(p2.x() - p1.x()) > 0) {
            double m = (p2.y() - p1.y()) / (p2.x() - p1.x());
            double a = qRadiansToDegrees(qAtan(m));
            double D1 = qAbs(p2.y() - point.y()) / qSin(qDegreesToRadians(a));
            double D2 = qAbs(point.y() - p1.y()) / qSin(qDegreesToRadians(a));

            if (qSin(qDegreesToRadians(a)) == 0) {
                return QPair<double, double>(qAbs(p2.x() - point.x()), qAbs(point.x() - p1.x()));
            }
            return QPair<double, double>(qAbs(D1), qAbs(D2));
        } else {
            return QPair<double, double>((qAbs(p2.y() - point.y())), (qAbs(point.y() - p1.y())));
        }
    }

    static QPointF DtoPoint1(double slope, QPointF point, double d1, double d2)
    {

        if (slope == qInf()) {

            return QPointF(point.x(), point.y() - d1);
        } else if (slope == 0) {
            return QPointF(point.x() - (d2 + d1), point.y());
        } else {
            double a = slope;
            double H = (d1 + d2) * qSin(qDegreesToRadians(a));
            double W = (d1 + d2) * qCos(qDegreesToRadians(a));

            return QPointF(point.x() + W, point.y() + H);
        }
    }
    static QPointF DtoPoint2(double slope, QPointF point, double d1, double d2)
    {

        if (slope == qInf()) {

            return QPointF(point.x(), point.y() + d2);
        } else if (slope == 0) {
            return QPointF(point.x() + d1, point.y());
        } else {

            double a = slope;

            double H1 = d1 * qSin(qDegreesToRadians(a));
            double W1 = d1 * qCos(qDegreesToRadians(a));

            double H = (d1 + d2) * qSin(qDegreesToRadians(a));
            double W = (d1 + d2) * qCos(qDegreesToRadians(a));

            return QPointF(point.x() + W1 - W, point.y() + H1 - H);
        }
    }

    static QPointF findSuperEllipse(QPoint a, QPoint b, QPoint middle, double n)
    {

        double find_n1 = 0, find_n2 = 0;

        QPoint center = QPoint(b.x(), a.y());
        a = QPoint(a.x() - center.x(), a.y() - center.y());
        b = QPoint(center.x() - b.x(), center.y() - b.y());
        middle = QPoint(middle.x() - center.x(), center.y() - middle.y());
        if (n > 0) {
            n = qAbs(n);

            double sum = area(a.x(), b.y(), n);

            double intg = b.y() * simpson_rule(a.x(), n, middle.x());

            double s1 = intg - (middle.x() * middle.y());
            double s2 = sum - intg;

            double min1 = 1;
            double diff = s1;
            for (double k = 1; k < 10; k += 0.01) {
                double sum1 = area(middle.x(), b.y() - middle.y(), k);
                if (qAbs(sum1 - s1) < diff) {
                    diff = qAbs(sum1 - s1);
                    min1 = k;
                }
            }
            double min2 = 1;
            diff = s2;
            for (double k = 1; k < 10; k += 0.01) {
                double sum1 = area(a.x() - middle.x(), middle.y(), k);
                if (qAbs(sum1 - s2) < diff) {
                    diff = qAbs(sum1 - s2);
                    min2 = k;
                }
            }

            return QPointF(min1, min2);
        }
    }

    static double simpson_rule(double a, double n, double x1)
    {
        double value;
        double upper_limit = x1;
        double lower_limit = 0;
        double interval_limit = 100;
        double interval_size = (upper_limit - lower_limit) / interval_limit;
        double sum = function(a, n, lower_limit) + function(a, n, upper_limit);
        for (int i = 1; i < interval_limit; i++) {
            if (i % 3 == 0)
                sum = sum + 2 * function(a, n, lower_limit + i * interval_size);
            else
                sum = sum + 3 * function(a, n, lower_limit + i * interval_size);
        }
        return (3 * interval_size / 8) * sum;
    }

    static double area(double a, double b, double n)
    {
        return a * b * qPow(tgamma(1 + 1 / n), 2) / tgamma(1 + 2 / n);
    }
    static double yCalculatorLast(QPointF p1, QPointF p2, double X)
    {

        if (p1.x() == -qInf()) {
            p1.setX(p2.x());
        }

        if (p2.x() == qInf()) {
            p2.setX(p1.x());
        }

        if (qAbs((p2.x() - p1.x())) > 0) {
            double m = (p2.y() - p1.y()) / (double)(p2.x() - p1.x());
            double Y = m * (X - p1.x()) + p1.y();
            return Y;
        } else {

            return p2.y();
        }
    }
    static double yCalculatorFirst(QPointF p1, QPointF p2, double X)
    {

        if (p1.x() == -qInf()) {
            p1.setX(p2.x());
        }

        if (p2.x() == qInf()) {
            p2.setX(p1.x());
        }

        if (qAbs((p2.x() - p1.x())) > 0) {
            double m = (p2.y() - p1.y()) / (double)(p2.x() - p1.x());
            double Y = m * (X - p1.x()) + p1.y();
            return Y;
        } else {

            return p1.y();
        }
    }

    static double computeSlopeMagnitude(QPointF a,
        QPointF b,
        double n,
        double m,
        double x,
        double y,
        double eps = 1e-3)
    {
        using std::fabs;
        using std::isfinite;
        using std::numeric_limits;

        const double INF = numeric_limits<double>::infinity();

        // --- 1) Try using X -> Y mapping (estimate dy/dx directly) ---
        double s_xy = INF;
        {
            QPointF p_minus = superEllipseXtoY(a, b, n, m, x - eps);
            QPointF p_plus = superEllipseXtoY(a, b, n, m, x + eps);

            if (isfinite(p_minus.x()) && isfinite(p_minus.y()) && isfinite(p_plus.x()) && isfinite(p_plus.y())) {
                double dx = p_plus.x() - p_minus.x(); // ~ 2*eps
                double dy = p_plus.y() - p_minus.y();

                if (fabs(dx) > 1e-12)
                    s_xy = fabs(dy / dx); // |dy/dx|
            }
        }

        // --- 2) Try using Y -> X mapping (estimate dx/dy, then invert) ---
        double s_yx = INF;
        {
            QPointF q_minus = superEllipseYtoX(a, b, n, m, y - eps);
            QPointF q_plus = superEllipseYtoX(a, b, n, m, y + eps);

            if (isfinite(q_minus.x()) && isfinite(q_minus.y()) && isfinite(q_plus.x()) && isfinite(q_plus.y())) {
                double dy = q_plus.y() - q_minus.y(); // ~ 2*eps
                double dx = q_plus.x() - q_minus.x();

                if (fabs(dy) > 1e-12) {
                    double dx_dy = dx / dy; // dx/dy
                    if (fabs(dx_dy) > 1e-12)
                        s_yx = 1.0 / fabs(dx_dy); // |dy/dx| = 1 / |dx/dy|
                }
            }
        }

        // --- 3) Pick a usable estimate ---
        double s = s_xy;
        if (!isfinite(s) || (isfinite(s_yx) && s_yx < s))
            s = s_yx;

        return s;
    }

    static QPointF superEllipseYtoX(QPointF a, QPointF b, double n, double m, double y)
    {

        if (n < 0 || m < 0) {
            n = qAbs(n);
            m = qAbs(m);

            double centerX = a.x();
            double centerY = -b.y();

            QPointF bb = QPointF(b.x(), -b.y());
            QPointF aa = QPointF(a.x(), -a.y());

            double A = qAbs(aa.y() - centerY);
            double B = qAbs(centerX - bb.x());

            double Y = -y;
            double X = -B * qPow(1 - qPow(qAbs((-Y + centerY) / (double)A), m), 1.0 / (double)n) + centerX;
            return QPointF(X, -Y);

        } else {
            n = qAbs(n);

            double centerX = b.x();
            double centerY = -a.y();

            QPointF bb = QPointF(b.x(), -b.y());
            QPointF aa = QPointF(a.x(), -a.y());

            double A = qAbs(aa.x() - centerX);
            double B = qAbs(centerY - bb.y());

            double Y = -y;
            double X = A * qPow(1 - qPow(qAbs((-Y + centerY) / (double)B), m), 1.0 / (double)n) + centerX;
            return QPointF(X, -Y);
        }
    }
    static QPointF superEllipseXtoY(QPointF a, QPointF b, double n, double m, double x)
    {

        if (n < 0 || m < 0) {
            n = qAbs(n);
            m = qAbs(m);

            double centerX = a.x();
            double centerY = b.y();

            QPointF bb = QPointF(b.x(), b.y());
            QPointF aa = QPointF(a.x(), a.y());

            double A = -(aa.y() - centerY);
            double B = -(centerX - bb.x());

            double X = x;
            double Y = A * qPow(1 - qPow(qAbs((x - centerX) / (double)B), n), 1.0 / (double)m) - centerY;
            return QPointF(X, -Y);

        } else {
            n = qAbs(n);

            double centerX = b.x();
            double centerY = a.y();

            QPointF bb = QPointF(b.x(), b.y());
            QPointF aa = QPointF(a.x(), a.y());

            double A = (aa.x() - centerX);
            double B = (centerY - bb.y());

            double X = x;
            double Y = B * qPow(1 - qPow(qAbs((x - centerX) / (double)A), n), 1.0 / (double)m) - centerY;
            return QPointF(X, -Y);
        }
    }

    static bool findNewRoot1(QPointF a, QPointF b, int cutX, double curve_n, double curve_m, float& p, float& q, float& r, float& s, float& y)
    {

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

        auto result = superEllipseXtoY(a, b, curve_n, curve_m,
            cutX);

        double x = result.x();
        y = result.y();

        if (qAbs(centerY - y) < 0.0001) {
            return false;
        }
        if (qAbs(x - centerX) < 0.0001) {
            return false;
        }
        findRoot(qAbs(curve_n), qAbs(curve_m), 0, 0, qAbs(A), qAbs(B), 250, x - centerX, centerY - y, p, q, r, s);

        if (curve_n < 0) {
            p = -p;
            q = -q;
            r = -r;
            s = -s;
        }

        return true;
    }

    static QPointF superEllipseXtoY(QPointF a, QPointF b, double n, double m, double x, double y)
    {

        if (n < 0 || m < 0) {
            n = qAbs(n);
            m = qAbs(m);

            double centerX = a.x();
            double centerY = b.y();

            QPointF bb = QPointF(b.x(), b.y());
            QPointF aa = QPointF(a.x(), a.y());

            double A = -(aa.y() - centerY);
            double B = -(centerX - bb.x());

            double X = x, Y = y;
            double degree = qRadiansToDegrees(qAtan(abs((-y) + centerY) / (double)abs(x - centerX)));

            // qDebug() << "degree" << degree << x << y;
            if (degree > 30) {
                Y = A * qPow(1 - qPow(qAbs((x - centerX) / (double)B), n), 1.0 / (double)m) - centerY;
                return QPointF(X, -Y);
            } else {
                X = B * qPow(1 - qPow(qAbs(((-y) + centerY) / (double)A), m), 1.0 / (double)n) + centerX;
                return QPointF(X, Y);
            }

        } else {
            n = qAbs(n);

            double centerX = b.x();
            double centerY = a.y();

            QPointF bb = QPointF(b.x(), b.y());
            QPointF aa = QPointF(a.x(), a.y());

            double A = (aa.x() - centerX);
            double B = (centerY - bb.y());

            double X = x, Y = y;
            double degree = qRadiansToDegrees(qAtan(abs((-y) + centerY) / (double)abs(x - centerX)));
            if (degree > 20) {

                Y = B * qPow(1 - qPow(qAbs((x - centerX) / (double)A), n), 1.0 / (double)m) - centerY;
                return QPointF(X, -Y);
            } else {
                X = A * qPow(1 - qPow(qAbs(((-y) + centerY) / (double)B), m), 1.0 / (double)n) + centerX;
                return QPointF(X, Y);
            }
        }
    }
    static double slope2(QPointF p1, QPointF p2, int& sign)
    {
        if (p1.x() == -qInf()) {
            p1.setX(p2.x());
        }

        if (p2.x() == qInf()) {
            p2.setX(p1.x());
        }

        double a;
        if (qAbs(p2.x() - p1.x()) > 0) {
            double m = (p2.y() - p1.y()) / (double)(p2.x() - p1.x());

            if (p2.x() - p1.x() < 0) {
                sign = -1;
            } else {
                sign = 1;
            }

            a = qRadiansToDegrees(qAtan(m));
        } else {
            a = qInf();
            if (p2.y() > p1.y()) {
                sign = 1;
            } else {
                sign = -1;
            }
        }

        return a;
    }
    static void distanceAndSlope(QPointF p1, QPointF p2, double& _distance, double& _slope, int& sign)
    {
        _distance = distance(p1, p2);
        _slope = slope2(p1, p2, sign);
    }

    static double round5(double value)
    {
        qint64 result = qRound64(value * 10000.0);
        return result / 10000.0;
    }

    static double round3(double value)
    {
        qint64 result = qRound64(value * 100.0);
        return result / 100.0;
    }

    static QPointF secondPoint(QPointF p, double distance, double slope, int sign)
    {
        if (slope == qInf()) {

            if (sign >= 1) {
                return QPointF(p.x(), p.y() + distance);
            } else {
                return QPointF(p.x(), p.y() - distance);
            }
        } else {
            double W = qCos(qDegreesToRadians(slope)) * distance;
            double H = qSin(qDegreesToRadians(slope)) * distance;
            if (sign >= 1) {
                return QPointF(p.x() + W, p.y() + H);
            } else {
                return QPointF(p.x() - W, p.y() - H);
            }
        }
    }

    static int findOuterLine(QVector<QLineF> lines, QPointF pos, int innerThreshold, int outerThreshold)
    {

        QPolygonF _polyInner;
        QPolygonF _polyOuter;
        QVector<int> list;
        for (int i = 0; i < lines.count(); i++) {
            MathHelper1::lineAreas(lines[i].p1(), lines[i].p2(), _polyInner, _polyOuter, innerThreshold, outerThreshold);

            if (_polyOuter.containsPoint(QPointF(pos.x(), pos.y()), Qt::WindingFill)) {

                list.append(i);
            }
        }
        if (list.size() == 0)
            return -1;

        QMap<int, double> values;
        for (int i = 0; i < list.size(); i++) {
            double dist = distanceFromLine(pos, lines[list[i]]);
            values[list[i]] = dist;
        }

        int min = values.keys()[0];
        QMapIterator<int, double> i(values);
        while (i.hasNext()) {
            i.next();

            if (i.value() < values[min]) {
                min = i.key();
            }
        }
        return min;
    }

    static double distanceFromLine(QPointF pos, QLineF line)
    {
        double m = (line.p2().y() - line.p1().y()) / (line.p2().x() - line.p1().x());
        double A = m;
        double B = -1;
        double C = -m * line.p1().x() + line.p1().y();

        return qAbs(pos.x() + pos.y() + C) / qSqrt(qPow(A, 2) + qPow(B, 2));
    }

    static double distanceFromLine2(QPointF pos, QLineF line)
    {
        // Extract the coefficients A, B, C from the line equation Ax + By + C = 0
        double A = line.dy();
        double B = -line.dx();
        double C = line.p1().x() * line.p2().y() - line.p2().x() * line.p1().y();

        // Coordinates of the point
        double x1 = pos.x();
        double y1 = pos.y();

        // Calculate the distance
        double distance = qAbs(A * x1 + B * y1 + C) / qSqrt(A * A + B * B);
        return distance;
    }

    static double lineEquationXtoY(QLineF line, double x)
    {
        double m = (line.p2().y() - line.p1().y()) / (line.p2().x() - line.p1().x());
        double y = m * (x - line.p1().y()) + line.p1().y();
        return y;
    }
};

#endif // MATHHELPER_H
