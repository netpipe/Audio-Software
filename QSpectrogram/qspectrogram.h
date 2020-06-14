#ifndef QSPECTROGRAM_H
#define QSPECTROGRAM_H

#include <QWidget>
#include "spectrogram.h"

class QSpectrogram : public QWidget
{
    Q_OBJECT
public:
    explicit QSpectrogram(Spectrogram *_spectrogram,
			  QWidget *parent = 0,
			  double _minFreq = 10.0f,
			  double _maxFreq = 21000.0f,
			  double _minAmpl = 1e-5,
			  double _maxAmpl = 1.0f);
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
signals:
public slots:
    void processData(float *buffer,
                     unsigned int bufferLength);
    void toggleColorbar(bool visible);
    void toggleWaveform(bool visible);
    void toggleSpectrum(bool visible);
    void toggleTimeGrid(bool visible);
    void toggleFreqGrid(bool visible);
    void toggleLogScaleFreq(bool logscale);
    void toggleLogScaleAmpl(bool logscale);

    void setMaxFreq(double maxFreq);
    void setMinFreq(double minFreq);
    void setMaxAmpl(double maxAmpl);
    void setMinAmpl(double minAmpl);

    void setLayoutMode(unsigned int layoutMode);
private:
    void drawGrid(QPainter &painter);
    void drawSpectrumPlot(QPainter &painter);
    void drawWaveformPlot(QPainter &painter);
    void drawColorbarPlot(QPainter &painter);
    void refreshPixmap();
    void renderImage(unsigned int newLines,
                     bool redraw);

    int freqToPixel(double freq);
    int timeToPixel(double time);

    // Limits of the plot;
    double minFreq;
    double maxFreq;
    double minAmpl;
    double maxAmpl;

    double freqTickBig;
    double freqTickSmall;

    unsigned int paddingX;
    unsigned int paddingY;
    unsigned int ylabelSpacing;
    unsigned int xlabelSpacing;

    unsigned int colorBarWidth;
    unsigned int spectrumWidth;
    unsigned int waveformWidth;

    unsigned int plotx, ploty, plotwidth, plotheight;

    double timeScroll;

    enum {
        DRAWMODE_OFF,
        DRAWMODE_SCROLL,
        DRAWMODE_SWEEP_SINGLE,
        DRAWMODE_SWEEP_REPEAT
    };

    enum {
        LAYOUT_HORIZONTAL,
        LAYOUT_VERTICAL
    };

    void evalColormap(float value, int &r, int &g, int &b);

    unsigned int drawMode;
    unsigned int layoutMode;
    bool logScaleFreq;
    bool logScaleAmpl;
    bool drawTimeGrid;
    bool drawFreqGrid;
    bool drawColorbar;
    bool drawSpectrum;
    bool drawWaveform;

    QImage *image;
    QPixmap pixmap;
    QColor backgroundColor;
    QColor gridColor;

    QVector<QVector<float> > colormap;

    Spectrogram *spectrogram;
};

#endif // QSPECTROGRAM_H
