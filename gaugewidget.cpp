#include "gaugewidget.h"
#include <chrono>
#include "pid.h"
#include <QDebug>

GaugeWidget::GaugeWidget(QWidget *parent) :
    QWidget(parent)
{
    setMinimumSize(250, 250);
}

BackgroundItem *GaugeWidget::addBackground(float position)
{
    BackgroundItem * item = new BackgroundItem(this);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

DegreesItem *GaugeWidget::addDegrees(float position)
{
    DegreesItem * item = new DegreesItem(this);
    item->setPosition(position);
    item->setColor(Qt::white);
    mItems.append(item);
    return item;
}


ValuesItem *GaugeWidget::addValues(float position)
{
    ValuesItem * item = new ValuesItem(this);
    item->setColor(Qt::white);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

ArcItem *GaugeWidget::addArc(float position)
{
    ArcItem * item = new ArcItem(this);
    item->setColor(Qt::white);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

ColorBand *GaugeWidget::addColorBand(float position)
{
    ColorBand * item = new ColorBand(this);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

NeedleItem *GaugeWidget::addNeedle(float position)
{
    NeedleItem * item = new NeedleItem(this);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

LabelItem *GaugeWidget::addLabel(float position)
{
    LabelItem * item = new LabelItem(this);
    item->setColor(Qt::white);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

GlassItem *GaugeWidget::addGlass(float position)
{
    GlassItem * item = new GlassItem(this);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

AttitudeMeter *GaugeWidget::addAttitudeMeter(float position)
{
    AttitudeMeter * item = new AttitudeMeter(this);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

void GaugeWidget::addItem(QcItem *item, float position)
{
    // takes parentship of the item
    item->setParent(this);
    item->setPosition(position);
    mItems.append(item);
}

int GaugeWidget::removeItem(QcItem *item)
{
    return mItems.removeAll(item);
}

QList<QcItem *> GaugeWidget::items()
{
    return mItems;
}


void GaugeWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    foreach(QcItem * item, mItems) {
        item->draw(&painter);
    }
}

QcItem::QcItem(QObject *parent) :
    QObject(parent)
{

    parentWidget = qobject_cast<QWidget*>(parent);
    mPosition = 50;
}

int QcItem::type()
{
    return 50;
}

void QcItem::update()
{
    parentWidget->update();
}

float QcItem::position()
{
    return mPosition;
}

QRectF QcItem::rect()
{
    return mRect;
}

void QcItem::setPosition(float position)
{
    if (position>100)
        mPosition = 100;
    else if (position<0)
        mPosition = 0;
    else
        mPosition = position;
    update();
}

QRectF QcItem::adjustRect(float percentage)
{
    float r = getRadius(mRect);
    float offset = r - (percentage*r) / 100.0;
    QRectF tmpRect = mRect.adjusted(offset, offset, -offset, -offset);
    return tmpRect;
}

float QcItem::getRadius(const QRectF &tmpRect)
{
    float r = 0;
    if (tmpRect.width()<tmpRect.height())
        r = tmpRect.width() / 2.0;
    else
        r = tmpRect.height() / 2.0;
    return r;
}

QRectF QcItem::resetRect()
{
    mRect = parentWidget->rect();
    float r = getRadius(mRect);
    mRect.setWidth(2.0*r);
    mRect.setHeight(2.0*r);
    mRect.moveCenter(parentWidget->rect().center());
    return mRect;
}

QPointF QcItem::getPoint(float deg, const QRectF &tmpRect)
{
    float r = getRadius(tmpRect);
    float xx = cos(qDegreesToRadians(deg))*r;
    float yy = sin(qDegreesToRadians(deg))*r;
    QPointF pt;
    xx = tmpRect.center().x() - xx;
    yy = tmpRect.center().y() - yy;
    pt.setX(xx);
    pt.setY(yy);
    return pt;
}



float QcItem::getAngle(const QPointF&pt, const QRectF &tmpRect)
{
    float xx = tmpRect.center().x() - pt.x();
    float yy = tmpRect.center().y() - pt.y();
    return qRadiansToDegrees(atan2(yy, xx));
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ScaleItem::ScaleItem(QObject *parent) :
    QcItem(parent)
{
    mMinDegree = -45;
    mMaxDegree = 225;
    mMinValue = 0;
    mMaxValue = 100;
}

void ScaleItem::setValueRange(float minValue, float maxValue)
{
    if (!(minValue < maxValue))
        throw(InvalidValueRange);
    else if (maxValue >= 240)
        over_s = true;
    mMinValue = minValue;
    mMaxValue = maxValue;

}

void ScaleItem::setDgereeRange(float minDegree, float maxDegree)
{
    if (!(minDegree<maxDegree))
        throw(InvalidValueRange);
    mMinDegree = minDegree;
    mMaxDegree = maxDegree;
}

float ScaleItem::getDegFromValue(float v)
{
    float a = (mMaxDegree - mMinDegree) / (mMaxValue - mMinValue);
    float b = -a*mMinValue + mMinDegree;
    return a*v + b;
}


void ScaleItem::setMinValue(float minValue)
{
    if (minValue>mMaxValue)
        throw (InvalidValueRange);
    mMinValue = minValue;
    update();
}


void ScaleItem::setMaxValue(float maxValue)
{
    if (maxValue<mMinValue)
        throw (InvalidValueRange);
    mMaxValue = maxValue;
    update();
}

void ScaleItem::setMinDegree(float minDegree)
{
    if (minDegree>mMaxDegree)
        throw (InvalidDegreeRange);
    mMinDegree = minDegree;
    update();
}
void ScaleItem::setMaxDegree(float maxDegree)
{
    if (maxDegree<mMinDegree)
        throw (InvalidDegreeRange);
    mMaxDegree = maxDegree;
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

BackgroundItem::BackgroundItem(QObject *parent) :
    QcItem(parent)
{
    setPosition(88);
    mPen = Qt::NoPen;
    setPosition(88);

    addColor(0.4, Qt::darkGray);
    addColor(0.8, Qt::black);

}


void BackgroundItem::draw(QPainter* painter)
{
    QRectF tmpRect = resetRect();
    painter->setBrush(Qt::NoBrush);
    QLinearGradient linearGrad(tmpRect.topLeft(), tmpRect.bottomRight());
    for (int i = 0; i<mColors.size(); i++) {
        linearGrad.setColorAt(mColors[i].first, mColors[i].second);
    }
    painter->setPen(mPen);
    painter->setBrush(linearGrad);
    painter->drawEllipse(adjustRect(position()));
}

void BackgroundItem::addColor(float position, const QColor &color)
{
    if (position<0 || position>1)
        return;
    QPair<float, QColor> pair;
    pair.first = position;
    pair.second = color;
    mColors.append(pair);
    update();
}

void BackgroundItem::clearrColors()
{
    mColors.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

GlassItem::GlassItem(QObject *parent) :
    QcItem(parent)
{
    setPosition(88);
}

void GlassItem::draw(QPainter *painter)
{
    resetRect();
    QRectF tmpRect1 = adjustRect(position());
    QRectF tmpRect2 = tmpRect1;
    float r = getRadius(tmpRect1);
    tmpRect2.setHeight(r / 2.0);
    painter->setPen(Qt::NoPen);

    QColor clr1 = Qt::gray;
    QColor clr2 = Qt::white;
    clr1.setAlphaF(0.2);
    clr2.setAlphaF(0.4);

    QLinearGradient linearGrad1(tmpRect1.topLeft(), tmpRect1.bottomRight());
    linearGrad1.setColorAt(0.1, clr1);
    linearGrad1.setColorAt(0.5, clr2);

    painter->setBrush(linearGrad1);
    painter->drawPie(tmpRect1, 0, 16 * 180);
    tmpRect2.moveCenter(rect().center());
    painter->drawPie(tmpRect2, 0, -16 * 180);
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

LabelItem::LabelItem(QObject *parent) :
    QcItem(parent)
{
    setPosition(50);
    mAngle = 270;
    mText = "%";
    mColor = Qt::black;
}

void LabelItem::draw(QPainter *painter)
{
    resetRect();
    QRectF tmpRect = adjustRect(position());
    float r = getRadius(rect());
    QFont font("Meiryo UI", r / 10.0, QFont::Bold);
    painter->setFont(font);
    painter->setPen(QPen(mColor));

    QPointF txtCenter = getPoint(mAngle, tmpRect);
    QFontMetrics fMetrics = painter->fontMetrics();
    QSize sz = fMetrics.size(Qt::TextSingleLine, mText);
    QRectF txtRect(QPointF(0, 0), sz);
    txtRect.moveCenter(txtCenter);

    painter->drawText(txtRect, Qt::TextSingleLine, mText);

}

void LabelItem::setAngle(float a)
{
    mAngle = a;
    update();
}

float LabelItem::angle()
{
    return mAngle;
}

void LabelItem::setText(const QString &text, bool repaint)
{
    mText = text;
    if (repaint)
        update();
}

QString LabelItem::text()
{
    return mText;
}

void LabelItem::setColor(const QColor &color)
{
    mColor = color;
    update();
}

QColor LabelItem::color()
{
    return mColor;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ArcItem::ArcItem(QObject *parent) :
    ScaleItem(parent)
{
    setPosition(80);
    mColor = Qt::black;
}

void ArcItem::draw(QPainter *painter)
{
    resetRect();
    QRectF tmpRect = adjustRect(position());
    float r = getRadius(tmpRect);

    QPen pen;
    pen.setColor(mColor);
    pen.setWidthF(r / 40);
    painter->setPen(pen);
    painter->drawArc(tmpRect, -16 * (mMinDegree + 180), -16 * (mMaxDegree - mMinDegree));
}

void ArcItem::setColor(const QColor &color)
{
    mColor = color;
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ColorBand::ColorBand(QObject *parent) :
    ScaleItem(parent)
{
    QColor tmpColor;
    tmpColor.setAlphaF(0.1);
    QPair<QColor, float> pair;

    pair.first = Qt::green;
    pair.second = 10;
    mBandColors.append(pair);

    pair.first = Qt::darkGreen;
    pair.second = 50;
    mBandColors.append(pair);

    pair.first = Qt::red;
    pair.second = 100;
    mBandColors.append(pair);

    setPosition(50);
}

QPainterPath ColorBand::createSubBand(float from, float sweep)
{
    QRectF tmpRect = adjustRect(position());
    QPainterPath path;
    path.arcMoveTo(tmpRect, 180 + from);
    path.arcTo(tmpRect, 180 + from, -sweep);
    return path;
}

void ColorBand::draw(QPainter *painter)
{
    resetRect();
    float r = getRadius(rect());
    QPen pen;
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidthF(r / 20.0);
    painter->setBrush(Qt::NoBrush);
    float offset = getDegFromValue(mBandStartValue);
    for (int i = 0; i<mBandColors.size(); i++) {
        QColor clr = mBandColors[i].first;
        float sweep;
        if (i == 0)
            sweep = getDegFromValue(mBandColors[i].second) - getDegFromValue(mMinValue);
        else
            sweep = getDegFromValue(mBandColors[i].second) - getDegFromValue(mBandColors[i - 1].second);
        QPainterPath path = createSubBand(-offset, sweep);
        offset += sweep;
        pen.setColor(clr);
        painter->setPen(pen);
        painter->drawPath(path);
    }
}
void ColorBand::setColors(const QList<QPair<QColor, float> > &colors)
{
    mBandColors = colors;
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

DegreesItem::DegreesItem(QObject *parent) :
    ScaleItem(parent)
{
    mStep = 10;
    mColor = Qt::black;
    mSubDegree = false;
    setPosition(90);
}


void DegreesItem::draw(QPainter *painter)
{
    resetRect();
    QRectF tmpRect = adjustRect(position());
    if (over_s)
    {
        for (float i = mMinValue; i <= mMaxValue; i++)
        {
            if (fmodf(mMaxValue, i) == 0)
            {
                int j = mMaxValue / i;
                if (j <= 9 && j >= 7)
                {
                    mStep = i;
                    break;
                }
                else continue;
            }
        }
    }
    else mStep = 10;
    painter->setPen(mColor);
    float r = getRadius(tmpRect);
    for (float val = mMinValue; val <= mMaxValue - mStep; val += mStep) {
        if (!(fmodf(val , 10) == 0))
        {
            val += 5;
        }
        print_degree(val, r, tmpRect, painter);
    }
    print_degree(mMaxValue, r, tmpRect, painter);
}

void DegreesItem::print_degree(float val  , float r , QRectF tmpRect , QPainter *painter)
{
    float deg = getDegFromValue(val);
    QPointF pt = getPoint(deg, tmpRect);
    QPainterPath path;
    path.moveTo(pt);
    path.lineTo(tmpRect.center());
    pt = path.pointAtPercent(0.03);
    QPointF newPt = path.pointAtPercent(0.13);
    QPen pen;
    pen.setColor(mColor);
    if (!mSubDegree)
        pen.setWidthF(r / 25.0);
    painter->setPen(pen);
    painter->drawLine(pt, newPt);
}

void DegreesItem::setStep(float step)
{
    mStep = step;
    update();
}

void DegreesItem::setColor(const QColor& color)
{
    mColor = color;
    update();
}

void DegreesItem::setSubDegree(bool b)
{
    mSubDegree = b;
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

NeedleItem::NeedleItem(QObject *parent) :
    ScaleItem(parent)
{
    mCurrentValue = 0;
    mColor = Qt::black;
    mLabel = nullptr;
    mNeedleType = FeatherNeedle;
}

void NeedleItem::draw(QPainter *painter)
{

    computeCurrentValue();

    resetRect();
    QRectF tmpRect = adjustRect(position());
    painter->save();
    painter->translate(tmpRect.center());
    float deg = getDegFromValue(mCurrentValue);
    painter->rotate(deg + 90.0);
    painter->setBrush(QBrush(mColor));
    painter->setPen(Qt::NoPen);

    QLinearGradient grad;

    switch (mNeedleType) {
    case NeedleItem::FeatherNeedle:
        createFeatherNeedle(getRadius(tmpRect));
        break;
    case NeedleItem::DiamonNeedle:
        createDiamonNeedle(getRadius(tmpRect));
        break;
    case NeedleItem::TriangleNeedle:
        createTriangleNeedle(getRadius(tmpRect));
        break;
    case NeedleItem::AttitudeMeterNeedle:
        createAttitudeNeedle(getRadius(tmpRect));
        break;
    case NeedleItem::CompassNeedle:
        createCompassNeedle(getRadius(tmpRect));
        grad.setStart(mNeedlePoly[0]);
        grad.setFinalStop(mNeedlePoly[1]);
        grad.setColorAt(0.9, Qt::red);
        grad.setColorAt(1, Qt::blue);
        painter->setBrush(grad);

        break;

    default:
        break;
    }
    painter->drawConvexPolygon(mNeedlePoly);
    painter->restore();
}



void NeedleItem::computeCurrentValue()
{
    using namespace std::chrono;

    if (!(std::abs(mCurrentValue - mTargetValue) > 0.001f))
    {
        return;
    }

    milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    auto timeSinceEpoch = ms.count();

    if (-1 != mNeedleLastMoved)
    {
        float currentTime = (timeSinceEpoch - mNeedleLastMoved) / 1000.0f;
        float direction = signum( mNeedleVelocity);

        if (std::abs(mNeedleVelocity) < 90.0f)
        {
            mNeedleAcceleration = 5.0f * (mTargetValue - mCurrentValue);
        }
        else
        {
            mNeedleAcceleration = 0.0f;
        }

        mNeedleAcceleration = 5.0f * (mTargetValue - mCurrentValue);
        mCurrentValue += mNeedleVelocity * currentTime;
        mNeedleVelocity += mNeedleAcceleration * currentTime;

        if ((mTargetValue - mCurrentValue) * direction < 0.001f * direction)
        {
            mCurrentValue = mTargetValue;
            mNeedleVelocity = 0.0f;
            mNeedleAcceleration = 0.0f;
            mNeedleLastMoved = -1;
        }
        else
        {
            mNeedleLastMoved = timeSinceEpoch;
        }

        if (mLabel != nullptr)
        {
            if(mCurrentValue < 0) mCurrentValue = 0.0f;
            mLabel->setText(QString::number(mCurrentValue, 'f', 0), false);
        }

        update();
    }
    else
    {
        mNeedleLastMoved = timeSinceEpoch;
        computeCurrentValue();
    }
}


void NeedleItem::setCurrentValue(float value)
{
    if (value < mMinValue)
        mTargetValue = mMinValue;
    else if (value > mMaxValue)
        mTargetValue = mMaxValue;
    else
        mTargetValue = value;

    update();
}

float NeedleItem::currentValue()
{
    return mCurrentValue;
}

void NeedleItem::setValueFormat(QString format) {
    mFormat = format;
    update();
}

QString NeedleItem::currentValueFormat() {
    return mFormat;
}

void NeedleItem::setColor(const QColor &color)
{
    mColor = color;
    update();
}

QColor NeedleItem::color()
{
    return mColor;
}

void NeedleItem::setLabel(LabelItem *label)
{
    mLabel = label;
    update();
}

LabelItem *NeedleItem::label()
{
    return mLabel;
}


void NeedleItem::setNeedle(NeedleItem::NeedleType needleType)
{
    mNeedleType = needleType;
    update();
}


void NeedleItem::createDiamonNeedle(float r)
{
    QVector<QPointF> tmpPoints;
    tmpPoints.append(QPointF(0.0, 0.0));
    tmpPoints.append(QPointF(-r / 20.0, r / 20.0));
    tmpPoints.append(QPointF(0.0, r));
    tmpPoints.append(QPointF(r / 20.0, r / 20.0));
    mNeedlePoly = tmpPoints;
}

void NeedleItem::createTriangleNeedle(float r)
{
    QVector<QPointF> tmpPoints;
    tmpPoints.append(QPointF(0.0, r));
    tmpPoints.append(QPointF(-r / 40.0, 0.0));
    tmpPoints.append(QPointF(r / 40.0, 0.0));
    mNeedlePoly = tmpPoints;
}

void NeedleItem::createFeatherNeedle(float r)
{
    QVector<QPointF> tmpPoints;
    tmpPoints.append(QPointF(0.0, r));
    tmpPoints.append(QPointF(-r / 40.0, 0.0));
    tmpPoints.append(QPointF(-r / 15.0, -r / 5.0));
    tmpPoints.append(QPointF(r / 15.0, -r / 5));
    tmpPoints.append(QPointF(r / 40.0, 0.0));
    mNeedlePoly = tmpPoints;
}

void NeedleItem::createAttitudeNeedle(float r)
{
    QVector<QPointF> tmpPoints;
    tmpPoints.append(QPointF(0.0, r));
    tmpPoints.append(QPointF(-r / 20.0, 0.85*r));
    tmpPoints.append(QPointF(r / 20.0, 0.85*r));
    mNeedlePoly = tmpPoints;
}

void NeedleItem::createCompassNeedle(float r)
{
    QVector<QPointF> tmpPoints;
    tmpPoints.append(QPointF(0.0, r));
    tmpPoints.append(QPointF(-r / 15.0, 0.0));
    tmpPoints.append(QPointF(0.0, -r));
    tmpPoints.append(QPointF(r / 15.0, 0.0));
    mNeedlePoly = tmpPoints;
}


ValuesItem::ValuesItem(QObject *parent) :
    ScaleItem(parent)
{
    setPosition(70);
    mColor = Qt::black;
    mStep = 10;
}


void ValuesItem::draw(QPainter*painter)
{
    QRectF  tmpRect = resetRect();
    float r = getRadius(adjustRect(99));
    QFont font("Meiryo UI", 0, QFont::Bold);
    font.setPointSizeF(0.08*r);
    painter->setFont(font);
    painter->setPen(mColor);
    if (over_s)
    {
        for (float i = mMinValue; i <= mMaxValue; i++)
        {
            if (fmodf(mMaxValue, i) == 0)
            {
                int j = mMaxValue / i;
                if (j <= 9 && j >= 7)
                {
                    mStep = i;
                    break;
                }
                else continue;
            }
        }
    }
    else mStep = 10;
    paint_numeber(mMinValue, tmpRect, painter);
    for (int val = mMinValue; val <= mMaxValue - mStep; val += mStep)
    {
        if (val % 10 == 0)
        {
            paint_numeber(val, tmpRect, painter);
        }
        else
        {
            val += 5;
            paint_numeber(val, tmpRect, painter);
        }
    }
    paint_numeber(mMaxValue, tmpRect, painter);

}

void ValuesItem::paint_numeber(int val , QRectF  tmpRect , QPainter* painter)
{
    float deg = getDegFromValue(val);
    QPointF pt = getPoint(deg, tmpRect);
    QPainterPath path;
    path.moveTo(pt);
    path.lineTo(tmpRect.center());
    QString strVal = QString::number(val);
    QFontMetrics fMetrics = painter->fontMetrics();
    QSize sz = fMetrics.size(Qt::TextSingleLine, strVal);
    QRectF txtRect(QPointF(0, 0), sz);
    QPointF textCenter = path.pointAtPercent(1.0 - position() / 100.0);
    txtRect.moveCenter(textCenter);
    painter->drawText(txtRect, Qt::TextSingleLine, strVal);
}

void ValuesItem::setStep(float step)
{
    mStep = step;
}


void ValuesItem::setColor(const QColor& color)
{
    mColor = color;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

AttitudeMeter::AttitudeMeter(QObject *parent) :
    QcItem(parent)
{
    mPitch = 0;
    mRoll = 0;
}

void AttitudeMeter::setCurrentPitch(float pitch)
{
    mPitch = -pitch;
    update();
}

void AttitudeMeter::setCurrentRoll(float roll)
{
    mRoll = roll;
    update();
}

QPointF AttitudeMeter::getIntersection(float r, const QPointF &pitchPoint, const QPointF &pt)
{
    // refrence it to zero

    Q_UNUSED(r)
    float a = (pitchPoint.y() - pt.y()) / (pitchPoint.x() - pt.x());
    float b = pt.y() - a*pt.x();
    return QPointF(0, a * 0 + b);
}

float AttitudeMeter::getStartAngle(const QRectF& tmpRect)
{
    float r = getRadius(tmpRect);
    QPointF pt1 = getPoint(mRoll, tmpRect);
    pt1.setY(pt1.y() - mPitchOffset);
    QPointF pitchPoint = QPointF(tmpRect.center().x(), tmpRect.center().y() - mPitchOffset);


    ///////////////////////////////////////
    QPainterPath path1;
    path1.moveTo(pitchPoint);
    path1.lineTo(getIntersection(r, pitchPoint, pt1) + QPointF(0, 5));
    path1.lineTo(getIntersection(r, pitchPoint, pt1) + QPointF(0, -5));

    QPainterPath path2;
    path2.addEllipse(tmpRect);

    QPointF p = path1.intersected(path2).pointAtPercent(.5);
    return getAngle(p, tmpRect);
}

void AttitudeMeter::draw(QPainter *painter)
{
    resetRect();
    QRectF tmpRect = adjustRect(position());
    float r = getRadius(tmpRect);
    if (mPitch<0)
        mPitchOffset = 0.0135*r*mPitch;
    else
        mPitchOffset = 0.015*r*mPitch;

    painter->setPen(Qt::NoPen);
    drawUpperEllipse(painter, tmpRect);
    drawLowerEllipse(painter, tmpRect);

    // Steps

    drawPitchSteps(painter, tmpRect);
    drawHandle(painter);

    drawDegrees(painter);

}

void AttitudeMeter::drawDegrees(QPainter *painter)
{
    resetRect();
    QRectF tmpRect = adjustRect(position());
    float r = getRadius(tmpRect);
    QPen pen;

    pen.setColor(Qt::white);
    painter->setPen(pen);
    for (int deg = 60; deg <= 120; deg += 10) {
        if (deg == 90)
            continue;
        drawDegree(painter, tmpRect, deg);
    }

    pen.setWidthF(r / 30.0);
    painter->setPen(pen);
    drawDegree(painter, tmpRect, 0);
    drawDegree(painter, tmpRect, 90);
    drawDegree(painter, tmpRect, 180);
    drawDegree(painter, tmpRect, 30);
    drawDegree(painter, tmpRect, 150);
}


void AttitudeMeter::drawDegree(QPainter * painter, const QRectF& tmpRect, float deg)
{
    QPointF pt1 = getPoint(deg, tmpRect);
    QPointF pt2 = tmpRect.center();
    QPainterPath path;
    path.moveTo(pt1);
    path.lineTo(pt2);
    QPointF pt = path.pointAtPercent(0.1);
    painter->drawLine(pt1, pt);
}


void AttitudeMeter::drawUpperEllipse(QPainter *painter, const QRectF &tmpRect)
{

    QLinearGradient radialGrad1(tmpRect.topLeft(), tmpRect.bottomRight());
    QColor clr1 = Qt::blue;
    clr1.setAlphaF(0.5);
    QColor clr2 = Qt::darkBlue;
    clr2.setAlphaF(0.5);
    radialGrad1.setColorAt(0, clr1);
    radialGrad1.setColorAt(.8, clr2);


    float offset = getStartAngle(tmpRect);
    float startAngle = 180 - offset;
    float endAngle = offset - 2 * mRoll;
    float span = endAngle - startAngle;

    painter->setBrush(radialGrad1);
    painter->drawChord(tmpRect, 16 * startAngle, 16 * span);

}


void AttitudeMeter::drawLowerEllipse(QPainter *painter, const QRectF &tmpRect)
{
    QLinearGradient radialGrad2(tmpRect.topLeft(), tmpRect.bottomRight());
    QColor clr1 = QColor(139, 119, 118);
    QColor clr2 = QColor(139, 119, 101);
    radialGrad2.setColorAt(0, clr1);
    radialGrad2.setColorAt(.8, clr2);

    float offset = getStartAngle(tmpRect);
    float startAngle = 180 + offset;
    float endAngle = offset - 2 * mRoll;
    float span = endAngle + startAngle;

    painter->setPen(Qt::NoPen);
    painter->setBrush(radialGrad2);
    painter->drawChord(tmpRect, -16 * startAngle, 16 * span);

}

void AttitudeMeter::drawPitchSteps(QPainter *painter, const QRectF &tmpRect)
{
    float r = getRadius(tmpRect);
    QPointF center = tmpRect.center();
    painter->save();
    painter->translate(center.x(), center.y() - mPitchOffset);
    painter->rotate(mRoll);
    QPen pen;
    pen.setColor(Qt::white);
    pen.setWidthF(r / 40.0);

    painter->setPen(pen);
    for (int i = -30; i <= 30; i += 10) {
        QPointF pt1;
        pt1.setX(-0.01*r*abs(i));
        pt1.setY(r / 70.0*i);
        QPointF pt2;
        pt2.setX(0.01*r*abs(i));
        pt2.setY(r / 70.0*i);
        painter->drawLine(pt1, pt2);

        if (i == 0)
            continue;

        // draw value
        QFont font("Meiryo UI", 0, QFont::Bold);
        font.setPointSizeF(0.08*r);
        painter->setFont(font);
        QString strVal = QString::number(abs(i));
        QFontMetrics fMetrics = painter->fontMetrics();
        QSize sz = fMetrics.size(Qt::TextSingleLine, strVal);
        QRectF leftTxtRect(QPointF(0, 0), sz);
        QRectF rightTxtRect(QPointF(0, 0), sz);
        leftTxtRect.moveCenter(pt1 - QPointF(0.1*r, 0));
        rightTxtRect.moveCenter(pt2 + QPointF(0.1*r, 0));
        painter->drawText(leftTxtRect, Qt::TextSingleLine, strVal);
        painter->drawText(rightTxtRect, Qt::TextSingleLine, strVal);
    }
    painter->restore();
}

void AttitudeMeter::drawHandle(QPainter *painter)
{
    QRectF tmpRct = adjustRect(15);
    float r = getRadius(tmpRct);
    QPen pen;
    pen.setColor(Qt::gray);
    pen.setWidthF(0.25*r);
    painter->setPen(pen);
    painter->drawArc(tmpRct, 0, -16 * 180);

    QPointF center = tmpRct.center();
    QPointF leftPt1 = center;
    QPointF leftPt2 = center;
    QPointF rightPt1 = center;
    QPointF rightPt2 = center;
    leftPt1.setX(center.x() - 2 * r);
    leftPt2.setX(center.x() - r);
    rightPt1.setX(center.x() + 2 * r);
    rightPt2.setX(center.x() + r);
    painter->drawLine(leftPt1, leftPt2);
    painter->drawLine(rightPt1, rightPt2);
    painter->drawEllipse(adjustRect(2));

    //
    QPointF pt1 = center;
    QPointF pt2 = center;
    // to get the real 100 % radius, without recomputing
    pt1.setY(center.y() + r);
    pt2.setY(center.y() + 4 * r);
    pen.setColor(Qt::gray);
    painter->setPen(pen);
    painter->drawLine(pt1, pt2);

    // trapezium
    painter->setPen(Qt::gray);
    painter->setBrush(Qt::gray);
    QPolygonF trapPoly;
    QPointF tmpPt = center;
    tmpPt.setX(center.x() - r);
    tmpPt.setY(center.y() + 4 * r);
    trapPoly.append(tmpPt);
    tmpRct = adjustRect(position());
    trapPoly.append(getPoint(290, tmpRct));
    trapPoly.append(getPoint(250, tmpRct));
    tmpPt = center;
    tmpPt.setX(center.x() + r);
    tmpPt.setY(center.y() + 4 * r);
    trapPoly.append(tmpPt);
    painter->drawPolygon(trapPoly);
    painter->drawChord(tmpRct, -16 * 70, -16 * 40);
}
