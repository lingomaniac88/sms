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
#include <Player/MarioAccess.hpp>

TMapWirePoint::TMapWirePoint()
{
	unk18 = 0.0f;
	unk20 = 0.0f;
	unk00.zero();
	unk0C.zero();
}

// fabricated but convenient for now
void TMapWirePoint::reset()
{
	unk00 = unk0C;
	unk18 = unk1C;
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

f32 TMapWire::getPointPowerAtReleased(f32 pos) const
{
	// 1 = default height, 0 = stretched all the way down
	f32 relativeHeightAtPos;
	if (pos >= mHangPosRatio) {
		relativeHeightAtPos = (pos - mHangPosRatio) / (1.0f - mHangPosRatio);
	} else {
		relativeHeightAtPos = 1.0f - pos / mHangPosRatio;
	}

	return 1.0f - relativeHeightAtPos * relativeHeightAtPos;
}

void TMapWire::getPointPosAtReleased(f32 pos, JGeometry::TVec3<f32>* out) const
{
	JGeometry::TVec3<f32> local_a4;
	getPointPosOnLine(pos, &local_a4);

	JGeometry::TVec3<f32> aTStack_98;
	getPointPosDefault(pos, &aTStack_98);

	f32 power = getPointPowerAtReleased(pos);
	f32 yAdjust
	    = power * mHangOrBouncePoint.y
	      + (1.0f - mBounceRemainingPower) * (aTStack_98.y - local_a4.y);

	out->set(local_a4.x, yAdjust + local_a4.y, local_a4.z);
}

void TMapWire::updatePointAtReleased(int index)
{
	TMapWirePoint* mapWirePoint = &mMapWirePoints[index];

	f32 pos = mapWirePoint->unk1C;
	if (fabsf(pos - mapWirePoint->unk18) > fabsf(mapWirePoint->unk20)) {
		pos = mapWirePoint->unk18 + mapWirePoint->unk20;
	}

	getPointPosAtReleased(pos, &mapWirePoint->unk00);
}

void TMapWire::updateMovePointAtReleased() { }

void TMapWire::initPointAtJustReleased(f32, TMapWirePoint*) { }

void TMapWire::release()
{
	if (mState == TMapWire::RELEASED) {
		return;
	}

	mState = TMapWire::RELEASED;

	mHangOrBouncePoint.zero();

	mNumActiveMapWirePoints = mNumMapWirePoints;

	int halfNumPoints = mNumActiveMapWirePoints / 2;

	for (int index = 0; index < halfNumPoints; index++) {
		TMapWirePoint& mapWirePoint = mMapWirePoints[index];

		mapWirePoint.reset();

		f32 pos            = mHangPosRatio / halfNumPoints * index;
		mapWirePoint.unk18 = pos;

		getPointPosAtReleased(pos, &mapWirePoint.unk00);

		mapWirePoint.unk20 = (mapWirePoint.unk1C - pos) / 1000.0f;
	}

	for (int index = halfNumPoints; index < mNumMapWirePoints; index++) {
		TMapWirePoint& mapWirePoint = mMapWirePoints[index];

		mapWirePoint.reset();

		f32 pos = (1.0f - mHangPosRatio) / (mNumMapWirePoints - halfNumPoints)
		          * (index - halfNumPoints + 1);
		mapWirePoint.unk18 = pos;

		getPointPosAtReleased(pos, &mapWirePoint.unk00);

		mapWirePoint.unk20 = (mapWirePoint.unk1C - pos) / 1000.0f;
	}

	f32 fVar1       = mStretchRate * fabsf(mHangPosRatio - 0.5f);
	f32 marioSpeedY = *gpMarioSpeedY;
	if (marioSpeedY > 0) {
		// TODO: This feels like an inlined helper method
		f32 marioSpeedSquared = *gpMarioSpeedX * *gpMarioSpeedX
		                        + marioSpeedY * marioSpeedY
		                        + *gpMarioSpeedZ * *gpMarioSpeedZ;
		f32 marioSpeed = JGeometry::TUtil<f32>::inv_sqrt(marioSpeedSquared)
		                 * marioSpeedSquared;
		mBounceAmplitude = mHeightRate * marioSpeed;
	} else {
		mBounceAmplitude = mReleaseHeight;
	}

	mBounceDecayRate      = mDownRateMax * fVar1;
	mBounceRemainingPower = 1.0f;
	mMoveTimer            = 1.0f;
}

void TMapWire::getPointPosAtHanged(f32 param_1,
                                   JGeometry::TVec3<f32>* out) const
{
	f32 fVar1 = param_1 - mHangPosRatio;

	out->x = mWireSpan.x * fVar1 + mHangOrBouncePoint.x;
	out->z = mWireSpan.z * fVar1 + mHangOrBouncePoint.z;

	if (param_1 <= mHangReferencePos1) {
		out->y = mHangOrBouncePoint.y
		         + ((mStartPoint.y - mHangOrBouncePoint.y)
		            * (mHangReferencePos1 - param_1))
		               / mHangReferencePos1;
	} else if (param_1 >= mHangReferencePos2) {
		out->y = mHangOrBouncePoint.y
		         + ((mEndPoint.y - mHangOrBouncePoint.y)
		            * (param_1 - mHangReferencePos2))
		               / (1.0f - mHangReferencePos2);
	} else {
		out->y = mHangOrBouncePoint.y;
	}
}

// TODO: What the heck is this anyway?
void TMapWire::getPointInfoAtHanged(f32 pos, TMapWirePoint* point)
{
	getPointPosAtHanged(pos, &point->unk00);
}

void TMapWire::setFootPointsAtHanged(MtxPtr mtx)
{
	mState = TMapWire::HANGING;

	mHangOrBouncePoint.set(mtx[0][3], mtx[1][3],
	                       mtx[2][3]); // translate portion of matrix
	mHangPosRatio = getPosInWire(mHangOrBouncePoint);

	mHangReferencePos1 = mHangPosRatio - mFootLength / unk30;
	mHangReferencePos2 = mHangPosRatio + mFootLength / unk30;

	mNumActiveMapWirePoints = 2;

	if (mFootLength < mHangPosRatio * unk30) {
		mMapWirePoints[0].unk18 = mHangReferencePos1;
		getPointInfoAtHanged(mHangReferencePos1, &mMapWirePoints[0]);
	} else {
		mMapWirePoints[0].unk18 = mHangPosRatio;
		mMapWirePoints[0].unk00.set(mHangOrBouncePoint);
	}

	if (mFootLength < (1.0f - mHangPosRatio) * unk30) {
		mMapWirePoints[1].unk18 = mHangReferencePos2;
		getPointInfoAtHanged(mHangReferencePos2, &mMapWirePoints[1]);
	} else {
		mMapWirePoints[1].unk18 = mHangPosRatio;
		mMapWirePoints[1].unk00.set(mHangOrBouncePoint);
	}
}

void TMapWire::calcViewAndDBEntry()
{
	mStartFittingModel->viewCalc();
	mEndFittingModel->viewCalc();
}

void TMapWire::move()
{
	bool bVar4;

	if (mState != TMapWire::RELEASED) {
		return;
	}

	mBounceRemainingPower -= mBounceDecayRate;

	if (mBounceRemainingPower < TMapWire::mEndRate) {
		bVar4 = true;
	} else {
		mMoveTimer += TMapWire::mMoveTimerSpeed;
		if (mMoveTimer >= 2.0f) {
			mMoveTimer -= 2.0f;
		}
		bVar4 = false;

		mHangOrBouncePoint.y = mBounceAmplitude * JMASCos(mMoveTimer * 32768.0f)
		                       * mBounceRemainingPower;
	}

	if (bVar4) {
		TMapWirePoint* mapWirePoint;

		for (int index = 0; index < mNumActiveMapWirePoints; index++) {
			mapWirePoint = &mMapWirePoints[index];
			mapWirePoint->reset();
		}

		mState = TMapWire::IDLE;
	} else {
		for (int index = 0; index < mNumActiveMapWirePoints; index++) {
			updatePointAtReleased(index);
		}
	}
}

f32 TMapWire::getPosInWire(const JGeometry::TVec3<f32>& param_1) const
{
	JGeometry::TVec3<f32> vecA = mStartPoint;
	JGeometry::TVec3<f32> vecB = mEndPoint;
	vecA.y                     = 0.0f;
	vecB.y                     = 0.0f;

	JGeometry::TVec3<f32> vecD = MsPerpendicFootToLineR(vecA, vecB, param_1);

	f32 totalLength   = (vecA - vecB).length();
	f32 partialLength = (vecD - vecA).length();
	return partialLength / totalLength;
}

/**
 * @brief Gets a position on the straight line connecting the wire's endpoints.
 *
 * @param pos the relative position on the wire (0 to 1)
 * @param out the output vector
 */
void TMapWire::getPointPosOnLine(f32 pos, JGeometry::TVec3<f32>* out) const
{
	out->x = mStartPoint.x + pos * mWireSpan.x;
	out->y = mStartPoint.y + pos * mWireSpan.y;
	out->z = mStartPoint.z + pos * mWireSpan.z;
}

void TMapWire::getPointPosOnWire(f32 param_1, JGeometry::TVec3<f32>* out) const
{
	if (param_1 < 0.0f) {
		param_1 = 0.0f;
	}
	if (param_1 > 1.0f) {
		param_1 = 1.0f;
	}

	if (mState == TMapWire::HANGING) {
		getPointPosAtHanged(param_1, out);
	} else {
		getPointPosAtReleased(param_1, out);
	}
}

/**
 * @brief The "default" position of a point on this wire after accounting for
 * its sag factor.
 *
 * @param pos the relative position on the wire (0 to 1)
 * @param out the output vector
 */
void TMapWire::getPointPosDefault(f32 pos, JGeometry::TVec3<f32>* out) const
{
	JGeometry::TVec3<f32> basePoint;
	getPointPosOnLine(pos, &basePoint);

	out->set(basePoint.x, basePoint.y - mWireSag * JMASSin(pos * 32768.0f),
	         basePoint.z);
}

void TMapWire::initTipPoints(const TCubeGeneralInfo*) { }

void TMapWire::init(const TCubeGeneralInfo* cubeInfo)
{
	mNumMapWirePoints = (s32)((cubeInfo->getUnk24().z / 50.0f + 1.0f) - 2.0f);
	mNumActiveMapWirePoints = mNumMapWirePoints;

	mMapWirePoints = new TMapWirePoint[mNumMapWirePoints];

	unk30 = cubeInfo->getUnk24().z;

	JGeometry::TVec3<f32> local_c4(0.0f, 0.0f, unk30 * 0.5f);

	JGeometry::TRotation3<TMtx33f> local_b8;
	local_b8.identity();
	local_b8.setEular((s16)(cubeInfo->getUnk18().x / 180.0f * 32768.0f),
	                  (s16)(cubeInfo->getUnk18().y / 180.0f * 32768.0f),
	                  (s16)(cubeInfo->getUnk18().z / 180.0f * 32768.0f));
	local_b8.mult33(local_c4);

	mStartPoint.x = cubeInfo->getUnkC().x - local_c4.x;
	mStartPoint.y = cubeInfo->getUnkC().y - local_c4.y + cubeInfo->getUnk24().y;
	mStartPoint.z = cubeInfo->getUnkC().z - local_c4.z;

	mEndPoint.x = local_c4.x + cubeInfo->getUnkC().x;
	mEndPoint.y = local_c4.y + cubeInfo->getUnkC().y + cubeInfo->getUnk24().y;
	mEndPoint.z = local_c4.z + cubeInfo->getUnkC().z;

	mWireSpan = mEndPoint - mStartPoint;

	mWireSag = cubeInfo->getUnk24().y * 0.5f;

	for (int i = 0; i < mNumMapWirePoints; i++) {
		TMapWirePoint& point = mMapWirePoints[i];

		f32 fVar15  = (f32)(i + 1) / (f32)(mNumMapWirePoints);
		point.unk18 = point.unk1C = fVar15;

		getPointPosDefault(point.unk18, &point.unk0C);

		point.unk00 = point.unk0C;
		point.unk18 = point.unk1C;
	}

	if (mEndPoint.x != mStartPoint.x) {
		f32 angle = atanf((mEndPoint.z - mStartPoint.z)
		                  / (mEndPoint.x - mStartPoint.x));
		unk34     = -angle * 180.0f / M_PI + 90.0f;
	} else {
		unk34 = 0.0f;
	}

	unk6C.set(mEndPoint.x - mStartPoint.x, mEndPoint.z - mStartPoint.z);
	unk6C.normalize();
	unk6C.rotate(M_PI / 2);

	mStartFittingModel
	    = SMS_CreatePartsModel("/common/map/WireFitting.bmd", 0x10210000);
	mEndFittingModel
	    = new J3DModel(mStartFittingModel->getModelData(), 0x10210000, 1);

	Mtx mtx;

	MsMtxSetXYZRPH(mtx, mStartPoint.x, mStartPoint.y, mStartPoint.z,
	               cubeInfo->getUnk18().x, cubeInfo->getUnk18().y,
	               cubeInfo->getUnk18().z);
	mStartFittingModel->setBaseTRMtx(mtx);
	mStartFittingModel->calc();

	MsMtxSetXYZRPH(mtx, mEndPoint.x, mEndPoint.y, mEndPoint.z,
	               cubeInfo->getUnk18().x, cubeInfo->getUnk18().y + 180.0f,
	               cubeInfo->getUnk18().z);
	mEndFittingModel->setBaseTRMtx(mtx);
	mEndFittingModel->calc();

	gpMapObjManager->entryStaticDrawBufferSun(mStartFittingModel);
	gpMapObjManager->entryStaticDrawBufferSun(mEndFittingModel);

	TMapCollisionStatic* collision1 = new TMapCollisionStatic;
	collision1->init("/common/map/WireFitting.col", 2, nullptr);
	MTXCopy(mStartFittingModel->getAnmMtx(0), collision1->unk20);
	collision1->setUp();

	TMapCollisionStatic* collision2 = new TMapCollisionStatic;
	collision2->init("/common/map/WireFitting.col", 2, nullptr);
	MTXCopy(mEndFittingModel->getAnmMtx(0), collision2->unk20);
	collision2->setUp();
}

TMapWire::TMapWire()
{
	unk34                   = 0.0f;
	mWireSag                = 0.0f;
	mNumActiveMapWirePoints = 0;
	mNumMapWirePoints       = 0;
	mMapWirePoints          = nullptr;
	mHangPosRatio           = 0.0f;
	mMoveTimer              = 0.0f;
	mBounceRemainingPower   = 0.0f;
	mBounceAmplitude        = 0.0f;
	mHangReferencePos1      = 0.0f;
	mHangReferencePos2      = 0.0f;
	mState                  = TMapWire::IDLE;
	mStartPoint.zero();
	mEndPoint.zero();
	mWireSpan.zero();
	unk6C.zero();
	mHangOrBouncePoint.zero();
	mStartFittingModel = nullptr;
	mEndFittingModel   = nullptr;
}
