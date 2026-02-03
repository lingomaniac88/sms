#include <types.h>

#include <Map/MapWire.hpp>

#include <JSystem/JMath.hpp>
#include <MarioUtil/MathUtil.hpp>

TMapWirePoint::TMapWirePoint()
{
	unk18 = 0.0f;
	unk20 = 0.0f;
	unk00.zero();
	unk0C.zero();
}

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

f32 TMapWire::getPointPowerAtReleased(f32 param_1) const
{
	f32 fVar1;
	if (param_1 >= unk4C) {
		fVar1 = (param_1 - unk4C) / (1.0f - unk4C);
	} else {
		fVar1 = 1.0f - param_1 / unk4C;
	}
	return 1.0f - fVar1 * fVar1;
}

void TMapWire::getPointPosAtReleased(f32, JGeometry::TVec3<f32>*) const { }

void TMapWire::updatePointAtReleased(int) { }

void TMapWire::updateMovePointAtReleased() { }

void TMapWire::initPointAtJustReleased(f32, TMapWirePoint*) { }

void TMapWire::release() { }

void TMapWire::getPointPosAtHanged(f32 param_1,
                                   JGeometry::TVec3<f32>* out) const
{
	f32 fVar1 = param_1 - unk4C;

	out->x = unk18.x * fVar1 + unk50;
	out->z = unk18.z * fVar1 + unk58;

	if (param_1 <= unk74) {
		out->y = unk54 + ((unk00.y - unk54) * (unk74 - param_1)) / unk74;
	} else if (param_1 >= unk78) {
		out->y
		    = unk54 + ((unk0C.y - unk54) * (param_1 - unk78)) / (1.0f - unk78);
	} else {
		out->y = unk54;
	}
}

void TMapWire::getPointInfoAtHanged(f32, TMapWirePoint*) { }

void TMapWire::setFootPointsAtHanged(MtxPtr) { }

void TMapWire::calcViewAndDBEntry()
{
	// TODO: Once we figure out what these are, replace void** with proper types
	typedef void (*FuncType)();

	FuncType* pFunc = (FuncType*)((u32)*unk3C + 0x14);
	(*pFunc)();

	pFunc = (FuncType*)((u32)*unk40 + 0x14);
	(*pFunc)();
}

void TMapWire::move()
{
	u8 padding[0x68];

	bool bVar4;

	if (unk7C != 2) {
		return;
	}

	unk60 -= unk68;

	if (unk60 < TMapWire::mEndRate) {
		bVar4 = true;
	} else {
		unk5C += TMapWire::mMoveTimerSpeed;
		if (unk5C >= 2.0f) {
			unk5C -= 2.0f;
		}
		bVar4 = false;

		unk54 = unk64 * JMASCos(unk5C * 32768.0f) * unk60;
	}

	if (bVar4) {
		TMapWirePoint* mapWirePoint;

		for (int idx = 0; idx < unk44; idx++) {
			mapWirePoint = &mMapWirePoints[idx];

			mapWirePoint->unk00 = mapWirePoint->unk0C;
			mapWirePoint->unk18 = mapWirePoint->unk1C;
		}

		unk7C = 0;
	} else {
		TMapWirePoint* mapWirePoint;

		for (int idx = 0; idx < unk44; idx++) {
			mapWirePoint = &mMapWirePoints[idx];

			f32 dVar10 = mapWirePoint->unk1C;
			if (fabsf(dVar10 - mapWirePoint->unk18)
			    > fabsf(mapWirePoint->unk20)) {
				dVar10 = mapWirePoint->unk18 + mapWirePoint->unk20;
			}

			JGeometry::TVec3<f32> local64;
			local64.scaleAdd(dVar10, unk00, unk18);

			JGeometry::TVec3<f32> local70;
			local70.set(dVar10 * unk18.x + unk00.x,
			            -(unk38 * JMASSin((u32)(dVar10 * 32768.0f))
			              - dVar10 * unk18.y + unk00.y),
			            dVar10 * unk18.z + unk00.z);

			f32 fVar2 = unk60;
			f32 fVar3 = unk68;

			mapWirePoint->unk00.x = local64.x;
			mapWirePoint->unk00.y = getPointPowerAtReleased(dVar10) * fVar3
			                        + (1.0f - fVar2) * (local70.y - local64.y)
			                        + local64.y;
			mapWirePoint->unk00.z = local64.z;
		}
	}
}

f32 TMapWire::getPosInWire(const JGeometry::TVec3<f32>& param_1) const
{
	JGeometry::TVec3<f32> vecA = unk00;
	JGeometry::TVec3<f32> vecB = unk0C;
	vecA.y                     = 0.0f;
	vecB.y                     = 0.0f;

	JGeometry::TVec3<f32> vecD = MsPerpendicFootToLineR(vecA, vecB, param_1);

	f32 totalLength   = (vecA - vecB).length();
	f32 partialLength = (vecD - vecA).length();
	return partialLength / totalLength;
}

void TMapWire::getPointPosOnLine(f32, JGeometry::TVec3<f32>*) const { }

void TMapWire::getPointPosOnWire(f32 param_1, JGeometry::TVec3<f32>* out) const
{
	if (param_1 < 0.0f) {
		param_1 = 0.0f;
	} else if (param_1 > 1.0f) {
		param_1 = 1.0f;
	}

	if (unk7C == 1) {
		getPointPosAtHanged(param_1, out);
	} else {
		f32 fVar6 = unk18.y * param_1 + unk00.y;
		f32 fVar4 = unk38;
		f32 fVar5 = JMASSin(param_1 * 32768.0f);

		out->x = unk18.x * param_1 + unk00.x;
		out->y = getPointPowerAtReleased(param_1) * unk54
		         + (1.0f - unk60) * (-(fVar4 * fVar5 - fVar6) - fVar6) + fVar6;
		out->z = unk18.z * param_1 + unk00.z;
	}
}

void TMapWire::getPointPosDefault(f32, JGeometry::TVec3<f32>*) const { }

void TMapWire::initTipPoints(const TCubeGeneralInfo*) { }

void TMapWire::init(const TCubeGeneralInfo* cubeInfo)
{
}

TMapWire::TMapWire()
{
	unk34          = 0.0f;
	unk38          = 0.0f;
	unk44          = 0;
	unk46          = 0;
	mMapWirePoints = nullptr;
	unk4C          = 0.0f;
	unk5C          = 0.0f;
	unk60          = 0.0f;
	unk64          = 0.0f;
	unk74          = 0.0f;
	unk78          = 0.0f;
	unk7C          = 0;
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
