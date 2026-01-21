#include <types.h>
#include <Map/MapWire.hpp>

TMapWirePoint::TMapWirePoint() { }

f32 TMapWire::mMoveTimerSpeed = 0.03f;
f32 TMapWire::mDownRateMax    = 0.003f;
f32 TMapWire::mEndRate        = 0.001f;
f32 TMapWire::mStretchRate    = 2.0f;
f32 TMapWire::mHeightRate     = 1.3f;
f32 TMapWire::mReleaseHeight  = 100.0f;
f32 TMapWire::mFootLength     = 26.0f;
f32 TMapWire::mDrawWidth      = 5.0f;
f32 TMapWire::mDrawHeight     = 6.0f;

void TMapWire::drawLower() const { }

void TMapWire::drawUpper() const { }

void TMapWire::getPointPowerAtReleased(f32) const { }

void TMapWire::getPointPosAtReleased(f32, JGeometry::TVec3<f32>*) const { }

void TMapWire::updatePointAtReleased(int) { }

void TMapWire::updateMovePointAtReleased() { }

void TMapWire::initPointAtJustReleased(f32, TMapWirePoint*) { }

void TMapWire::release() { }

void TMapWire::getPointPosAtHanged(f32, JGeometry::TVec3<f32>*) const { }

void TMapWire::getPointInfoAtHanged(f32, TMapWirePoint*) { }

void TMapWire::setFootPointsAtHanged(MtxPtr) { }

void TMapWire::calcViewAndDBEntry() { }

void TMapWire::move() { }

f32 TMapWire::getPosInWire(const JGeometry::TVec3<f32>&) const { }

void TMapWire::getPointPosOnLine(f32, JGeometry::TVec3<f32>*) const { }

void TMapWire::getPointPosOnWire(f32, JGeometry::TVec3<f32>*) const { }

void TMapWire::getPointPosDefault(f32, JGeometry::TVec3<f32>*) const { }

void TMapWire::initTipPoints(const TCubeGeneralInfo*) { }

void TMapWire::init(const TCubeGeneralInfo*) { }

TMapWire::TMapWire()
{
	unk34 = 0.0f;
	unk38 = 0.0f;
	unk44 = 0;
	unk46 = 0;
	unk48 = 0;
	unk4C = 0.0f;
	unk5C = 0.0f;
	unk60 = 0.0f;
	unk64 = 0.0f;
	unk74 = 0.0f;
	unk78 = 0.0f;
	unk7C = 0;
	unk00.zero();
	unk0C.zero();
	unk18.zero();
	unk70 = 0.0f;
	unk6C = 0.0f;
	unk58 = 0.0f;
	unk54 = 0.0f;
	unk50 = 0.0f;
	unk3C = nullptr;
	unk40 = nullptr;
}
