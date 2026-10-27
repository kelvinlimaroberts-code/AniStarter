#include "CommonStates.h"


#include "Sprite.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Sprite& Sprite::operator=(const Sprite& rhs) {
	mPos = rhs.mPos;
	mVel = rhs.mVel;
	depth = rhs.depth;
	mTexRect = rhs.mTexRect;
	mAnimRect = rhs.mAnimRect;
	mAnimFrame = rhs.mAnimFrame;
	mAnimationSpeed = rhs.mAnimationSpeed;
	colour = rhs.colour;
	rotation = rhs.rotation;
	scale = rhs.scale;
	origin = rhs.origin;
	mpTex = rhs.mpTex;
	mpTexData = rhs.mpTexData;
	return *this;
}
void Sprite::Draw(SpriteBatch& batch)
{
	if (isAnimated)
	{
		batch.Draw(mpTex, mPos, &(RECT)mAnimFrame, colour, rotation, origin, scale, DirectX::SpriteEffects::SpriteEffects_None, depth);
	}
	else
	{
		batch.Draw(mpTex, mPos, &(RECT)mTexRect, colour, rotation, origin, scale, DirectX::SpriteEffects::SpriteEffects_None, depth);
	}
}
void Sprite::SetTex(ID3D11ShaderResourceView& tex, const RECTF& texRect)
{
	mpTex = &tex;
	mTexRect = texRect;
	mAnimFrame = texRect;
	mpTexData = &mD3D.GetCache().Get(mpTex);
	
	if (mTexRect.left == mTexRect.right && mTexRect.top == mTexRect.bottom)
	{
		SetTexRect(RECTF{ 0,0,mpTexData->dim.x,mpTexData->dim.y });
	}
}
void Sprite::SetTexRect(const RECTF& texRect) {
	mTexRect = texRect;
	mAnimFrame = texRect;
}
void Sprite::Scroll(float x, float y) {
	mTexRect.left += x;
	mTexRect.right += x;
	mTexRect.top += y;
	mTexRect.bottom += y;
}

void Sprite::SetAnimRect(const RECTF& animRect)
{
	mAnimRect = animRect;
}

void Sprite::Animate(float elapsed)
{
	mAnimationTimer += elapsed;                 //add to the timer
	if (mAnimationTimer >= mAnimationSpeed)     //detect if over the animation timer
	{
		mAnimationTimer = 0.f;					//reset the timer
		mAnimFrame.left += mTexRect.right;		//move one frame to the right
		mAnimFrame.right += mTexRect.right;

		if (mAnimFrame.right >= mAnimRect.right)  //if attempting to animate past the mAnimRect  on the x axis
		{
			mAnimFrame.left = mAnimRect.left;   //reset the left/right frame animation back to frame one
			mAnimFrame.right = mTexRect.right;

			mAnimFrame.top += mTexRect.bottom;  //move the up/down animation to the next row
			mAnimFrame.bottom += mTexRect.bottom;

			if (mAnimFrame.bottom >= mAnimRect.bottom)//if attempting to animate past the mAnimrect on the y axis
			{
				mAnimFrame.top = mAnimRect.top; //reset the up/down frame animation to frame one   
				mAnimFrame.bottom = mTexRect.bottom;
			}
		}
	}
	
}

void Sprite::SetAnimationSpeed(float speed)
{
	mAnimationSpeed = speed;
}


