#ifndef QCGAUGEWIDGET_H
#define QCGAUGEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QObject>
#include <QRectF>
#include <QtMath>



#if defined(QCGAUGE_COMPILE_LIBRARY)
#  define QCGAUGE_DECL  Q_DECL_EXPORT
#elif defined(QCGAUGE_USE_LIBRARY)
#  define QCGAUGE_DECL Q_DECL_IMPORT
#else
#  define QCGAUGE_DECL
#endif

class GaugeWidget;
class QcItem;
class BackgroundItem;
class DegreesItem;
class ValuesItem;
class ArcItem;
class ColorBand;
class NeedleItem;
class LabelItem;
class GlassItem;
class AttitudeMeter;

class QCGAUGE_DECL GaugeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GaugeWidget(QWidget *parent = 0);

    BackgroundItem* addBackground(float position);
    DegreesItem* addDegrees(float position);
    ValuesItem* addValues(float position);
    ArcItem* addArc(float position);
    ColorBand* addColorBand(float position);
    NeedleItem* addNeedle(float position);
    LabelItem* addLabel(float position);
    GlassItem* addGlass(float position);
    AttitudeMeter* addAttitudeMeter(float position);


    void addItem(QcItem* item, float position);
    int removeItem(QcItem* item);
    QList <QcItem*> items();
    QList <QcItem*> mItems;


signals:

    public slots :
private:
    void paintEvent(QPaintEvent *);

};

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

class QCGAUGE_DECL QcItem : public QObject
{
    Q_OBJECT
public:
    explicit QcItem(QObject *parent = 0);
    virtual void draw(QPainter *) = 0;
    virtual int type();

    void setPosition(float percentage);
    float position();
    QRectF rect();
    enum Error { InvalidValueRange, InvalidDegreeRange, InvalidStep };


protected:
    QRectF adjustRect(float percentage);
    float getRadius(const QRectF &);
    float getAngle(const QPointF&, const QRectF &tmpRect);
    QPointF getPoint(float deg, const QRectF &tmpRect);
    QRectF resetRect();
    void update();

private:
    QRectF mRect;
    QWidget *parentWidget;
    float mPosition;
};
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

class QCGAUGE_DECL ScaleItem : public QcItem
{
    Q_OBJECT
public:
    explicit ScaleItem(QObject *parent = 0);

    void setValueRange(float minValue, float maxValue);
    void setDgereeRange(float minDegree, float maxDegree);
    void setMinValue(float minValue);
    void setMaxValue(float maxValue);
    void setMinDegree(float minDegree);
    void setMaxDegree(float maxDegree);

signals:

    public slots :
protected:

    float getDegFromValue(float);
    bool over_s;
    float mMinValue;
    float mMaxValue;
    float mMinDegree;
    float mMaxDegree;

};
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

class QCGAUGE_DECL BackgroundItem : public QcItem
{
    Q_OBJECT
public:
    explicit BackgroundItem(QObject *parent = 0);
    void draw(QPainter*);
    void addColor(float position, const QColor& color);
    void clearrColors();


private:
    QPen mPen;
    QList<QPair<float, QColor> > mColors;
    QLinearGradient mLinearGrad;
};
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

class GlassItem : public QcItem
{
    Q_OBJECT
public:
    explicit GlassItem(QObject *parent = 0);
    void draw(QPainter*);
};



///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

class QCGAUGE_DECL LabelItem : public QcItem
{
    Q_OBJECT
public:
    explicit LabelItem(QObject *parent = 0);
    virtual void draw(QPainter *);
    void setAngle(float);
    float angle();
    void setText(const QString &text, bool repaint = true);
    QString text();
    void setColor(const QColor& color);
    QColor color();

private:
    float mAngle;
    QString mText;
    QColor mColor;
};

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

class QCGAUGE_DECL ArcItem : public ScaleItem
{
    Q_OBJECT
public:
    explicit ArcItem(QObject *parent = 0);
    void draw(QPainter*);
    void setColor(const QColor& color);

private:
    QColor mColor;

signals:

    public slots :


};
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

class QCGAUGE_DECL ColorBand : public ScaleItem
{
    Q_OBJECT
public:
    explicit ColorBand(QObject *parent = 0);
    void draw(QPainter*);
    void setColors(const QList<QPair<QColor, float> >& colors);

private:
    QPainterPath createSubBand(float from, float sweep);

    QList<QPair<QColor, float> > mBandColors;
    float mBandStartValue;
};
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
class QCGAUGE_DECL DegreesItem : public ScaleItem
{
    Q_OBJECT
public:
    explicit DegreesItem(QObject *parent = 0);
    void draw(QPainter *painter);
    void setStep(float step);
    void setColor(const QColor& color);
    void setSubDegree(bool);
    void print_degree(float val, float r, QRectF tmpRect, QPainter *painter);
private:
    float mStep;
    QColor mColor;
    bool mSubDegree;
};
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

class QCGAUGE_DECL NeedleItem : public ScaleItem
{
    Q_OBJECT
public:
    explicit NeedleItem(QObject *parent = 0);
    void draw(QPainter*);
    void setCurrentValue(float value);
    void computeCurrentValue();
    float currentValue();
    void setValueFormat(QString format);
    QString currentValueFormat();
    void setColor(const QColor & color);
    QColor color();

    void setLabel(LabelItem*);
    LabelItem * label();

    enum NeedleType { DiamonNeedle, TriangleNeedle, FeatherNeedle, AttitudeMeterNeedle, CompassNeedle };//#

    void setNeedle(NeedleItem::NeedleType needleType);
private:
    QPolygonF mNeedlePoly;
    float mCurrentValue{0.f};
    float mTargetValue{0.f};
    float mNeedleAcceleration{0.f};
    float mNeedleVelocity{0.f};
    long long mNeedleLastMoved{-1};
    QColor mColor;
    void createDiamonNeedle(float r);
    void createTriangleNeedle(float r);
    void createFeatherNeedle(float r);
    void createAttitudeNeedle(float r);
    void createCompassNeedle(float r);
    NeedleType mNeedleType;
    LabelItem *mLabel;
    QString mFormat;
};
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


class QCGAUGE_DECL ValuesItem : public ScaleItem
{
    Q_OBJECT
public:
    explicit ValuesItem(QObject *parent = 0);
    void draw(QPainter*);
    void setStep(float step);
    void setColor(const QColor& color);
    void paint_numeber(int val, QRectF  tmpRect , QPainter*painter);
private:
    float mStep;
    QColor mColor;
};
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

class QCGAUGE_DECL AttitudeMeter : public QcItem
{
    Q_OBJECT
public:
    explicit AttitudeMeter(QObject *parent = 0);

    void draw(QPainter *);
    void setCurrentPitch(float pitch);
    void setCurrentRoll(float roll);
private:
    float mRoll;
    float mPitch;
    float mPitchOffset;

    QPolygonF mHandlePoly;
    QPainterPath mStepsPath;

    QPointF getIntersection(float r, const QPointF& pitchPoint, const QPointF& pt);
    float getStartAngle(const QRectF& tmpRect);

    void drawDegrees(QPainter *);
    void drawDegree(QPainter * painter, const QRectF& tmpRect, float deg);
    void drawUpperEllipse(QPainter *, const QRectF&);
    void drawLowerEllipse(QPainter *, const QRectF&);
    void drawPitchSteps(QPainter *, const QRectF&);
    void drawHandle(QPainter *);
    void drawSteps(QPainter *, float);

};

#endif // QCGAUGEWIDGET_H
