#include <dolphin/mtx.h>
#include <fake_tgmath.h>
#include <types.h>

#include <Map/MapWire.hpp>

#include <Camera/CubeMapTool.hpp>
#include <JSystem/JMath.hpp>
#include <Map/MapCollisionEntry.hpp>
#include <MarioUtil/MathUtil.hpp>
#include <MarioUtil/ModelUtil.hpp>
#include <MoveBG/MapObjManager.hpp>

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

	out->x = unk18.x * fVar1 + unk50.x;
	out->z = unk18.z * fVar1 + unk50.z;

	if (param_1 <= unk74) {
		out->y = unk50.y + ((unk00.y - unk50.y) * (unk74 - param_1)) / unk74;
	} else if (param_1 >= unk78) {
		out->y
		    = unk50.y + ((unk0C.y - unk50.y) * (param_1 - unk78)) / (1.0f - unk78);
	} else {
		out->y = unk50.y;
	}
}

void TMapWire::getPointInfoAtHanged(f32, TMapWirePoint*) { }

void TMapWire::setFootPointsAtHanged(MtxPtr) { }

void TMapWire::calcViewAndDBEntry()
{
	unk3C->viewCalc();
	unk40->viewCalc();
}

void TMapWire::move()
{
	u8 padding[0x30];

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

		unk50.y = unk64 * JMASCos(unk5C * 32768.0f) * unk60;
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
		out->y = getPointPowerAtReleased(param_1) * unk50.y
		         + (1.0f - unk60) * (-(fVar4 * fVar5 - fVar6) - fVar6) + fVar6;
		out->z = unk18.z * param_1 + unk00.z;
	}
}

void TMapWire::getPointPosDefault(f32, JGeometry::TVec3<f32>*) const { }

void TMapWire::initTipPoints(const TCubeGeneralInfo*) { }

void TMapWire::init(const TCubeGeneralInfo* cubeInfo)
{
	s32 iVar9 = (s32)((cubeInfo->getUnk24().z / 50.0f + 1.0f) - 2.0f);
	unk46     = iVar9;
	unk44     = unk46;

	mMapWirePoints = new TMapWirePoint[unk46];

	unk30 = cubeInfo->getUnk24().z;

	JGeometry::TVec3<f32> local_c4(0.0f, 0.0f, unk30 * 0.5f);

	JGeometry::TRotation3<TMtx33f> local_b8;
	local_b8.ref(0, 2) = local_b8.ref(1, 2) = 0.0f;
	local_b8.ref(0, 1) = local_b8.ref(2, 1) = 0.0f;
	local_b8.ref(1, 0) = local_b8.ref(2, 0) = 0.0f;
	local_b8.ref(0, 0) = local_b8.ref(1, 1) = local_b8.ref(2, 2) = 1.0f;
	local_b8.setEular((s16)(cubeInfo->getUnk18().x / 180.0f * 32768.0f),
	                  (s16)(cubeInfo->getUnk18().y / 180.0f * 32768.0f),
	                  (s16)(cubeInfo->getUnk18().z / 180.0f * 32768.0f));

	// TODO: What the heck is going on with these inlines?!
	TMtx33f::multiplyInPlaceNest2(local_b8, &local_c4);

	unk00.x = cubeInfo->getUnkC().x - local_c4.x;
	unk00.y = cubeInfo->getUnkC().y - local_c4.y + cubeInfo->getUnk24().y;
	unk00.z = cubeInfo->getUnkC().z - local_c4.z;

	unk0C.x = local_c4.x + cubeInfo->getUnkC().x;
	unk0C.y = local_c4.y + cubeInfo->getUnkC().y + cubeInfo->getUnk24().y;
	unk0C.z = local_c4.z + cubeInfo->getUnkC().z;

	unk18 = unk0C - unk00;

	unk38 = cubeInfo->getUnk24().y * 0.5f;

	for (int i = 0; i < unk46; i++) {
		TMapWirePoint& point = mMapWirePoints[i];

		f32 fVar15  = (f32)(i + 1) / (f32)(unk46);
		point.unk18 = point.unk1C = fVar15;

		point.unk0C.x = unk18.x * point.unk18 + unk00.x;
		point.unk0C.y = unk18.y * point.unk18 + unk00.y - unk38 * JMASSin(point.unk18 * 32768.0f);
		point.unk0C.z = unk18.z * point.unk18 + unk00.z;

		point.unk00 = point.unk0C;
		point.unk18 = point.unk1C;
	}

	if (unk0C.x != unk00.x) {
		f32 angle = atanf((unk0C.z - unk00.z) / (unk0C.x - unk00.x));
		unk34 = -angle * 180.0f / M_PI + 90.0f;
	} else {
		unk34 = 0.0f;
	}

	unk6C.set(unk0C.x - unk00.x, unk0C.z - unk00.z);
	unk6C.normalize();
	unk6C.rotate(M_PI / 2);

	unk3C = SMS_CreatePartsModel("/common/map/WireFitting.bmd", 0x10210000);
	unk40 = new J3DModel(unk3C->getModelData(), 0x10210000, 1);

	Mtx mtx;

	MsMtxSetXYZRPH(mtx, unk00.x, unk00.y, unk00.z, unk18.x, unk18.y, unk18.z);
	unk3C->setBaseTRMtx(mtx);
	unk3C->calc();

	MsMtxSetXYZRPH(mtx, unk0C.x, unk0C.y, unk0C.z, unk18.x, unk18.y + 180.0f,
	                unk18.z);
	unk40->setBaseTRMtx(mtx);
	unk40->calc();

	gpMapObjManager->entryStaticDrawBufferSun(unk3C);
	gpMapObjManager->entryStaticDrawBufferSun(unk40);

	TMapCollisionStatic* collision1 = new TMapCollisionStatic;
	collision1->init("/common/map/WireFitting.col", 2, nullptr);
	MTXCopy(unk3C->getAnmMtx(0), collision1->unk20);
	collision1->setUp();

	TMapCollisionStatic* collision2 = new TMapCollisionStatic;
	collision2->init("/common/map/WireFitting.col", 2, nullptr);
	MTXCopy(unk40->getAnmMtx(0), collision2->unk20);
	collision2->setUp();
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
	unk6C.zero();
	unk50.zero();
	unk3C = nullptr;
	unk40 = nullptr;
}
