#ifndef WARP_H
#define WARP_H

class Warp {
public:
    Warp(double base_t0,
        double base_t1,
        double t0,
        double t1,
        double pps,
        double width,
        double tempo = 120)
    {
        _base_t0 = base_t0;
        _bsase_t1 = base_t1;
        _t1 = t1;
        _t0 = t0;
        _pps = pps;
        _width = width;
        _tempo = tempo;
    }
    Warp()
    {
    }

    double base_t0() const;
    void setBase_t0(double base_t0);

    double bsase_t1() const;
    void setBsase_t1(double bsase_t1);

    double t0() const;
    void setT0(double t0);

    double t1() const;
    void setT1(double t1);

    double pps() const;
    void setPps(double pps);

    double width() const;
    void setWidth(double width);

    double tempo() const;
    void setTempo(double tempo);

private:
    double _base_t0;
    double _bsase_t1;
    double _t0;
    double _t1;
    double _pps;
    double _width;
    double _tempo = 120;
};

#endif // WARP_H
