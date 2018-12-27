#ifndef FACE_NORMAL_TRAIT_H
#define FACE_NORMAL_TRAIT_H

#include "Trait.h"

namespace MeshLib {
#define f_normal(f) trait<FaceNormalTrait,Face>(f).normal()

class FaceNormalTrait : public Trait {
public:
	FaceNormalTrait(){ };
	~FaceNormalTrait(){ };
	void read(){};
	void write(){};

	Point & normal(){ return m_normal;};

private:
	Point m_normal;
};

}

#endif
