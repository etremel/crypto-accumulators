/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#include <bilinear/GT_DCLXVI.hpp>
#include <utils/Pointers.hpp>

GTDCLXVI::GTDCLXVI() {
}

GTDCLXVI::GTDCLXVI(const GTDCLXVI& other) {
    fp12e_t eObj;
    other.exportObject(eObj);
    fp12e_set(_fp12e, eObj);
}

GT& GTDCLXVI::operator=(const GT& other) {
    if(this == &other) {
        return *this;
    } else {
        GTDCLXVI* pGT = (GTDCLXVI*)&other;
        fp12e_t eObj;
        pGT->exportObject(eObj);
        fp12e_set(_fp12e, eObj);
        return *this;
    }
}

void GTDCLXVI::doMultiplication(const GT& other, GT& result) {
    const GTDCLXVI& otherGT = ref_cast<GTDCLXVI>(other);
    fp12e_t op, rop;
    otherGT.exportObject(op);
    fp12e_mul(rop, _fp12e, op);
    GTDCLXVI& resultGT = ref_cast<GTDCLXVI>(result);
    resultGT.importObject(rop);
}

void GTDCLXVI::doPower(const Scalar& scalar, GT& result) {
    const ScalarDCLXVI& pScalar = ref_cast<ScalarDCLXVI>(scalar);
    scalar_t cScalar;
    pScalar.exportObject(cScalar);
    fp12e_t rop;
    fp12e_pow_vartime(rop, _fp12e, cScalar);
    GTDCLXVI& pGT = ref_cast<GTDCLXVI>(result);
    pGT.importObject(rop);
}

int GTDCLXVI::isEqual(const GT& other) {
    const GTDCLXVI& pGT = ref_cast<GTDCLXVI>(other);
    fp12e_t eObj;
    pGT.exportObject(eObj);
    return fp12e_iseq(_fp12e, eObj);
}

void GTDCLXVI::importObject(const void* obj) {
    fp12e_struct_t* op = (fp12e_struct_t*)obj;
    fp12e_set(_fp12e, op);
}

void GTDCLXVI::exportObject(void* obj) const {
    fp12e_struct_t* rop = (fp12e_struct_t*)obj;
    fp12e_set(rop, _fp12e);
}

void GTDCLXVI::readFromFile(std::istream& inFile) {
    for(int i = 0; i < 24; i++) {
        inFile.read((char*)&_fp12e->m_a->m_a->v[i], sizeof(mydouble));
        inFile.read((char*)&_fp12e->m_a->m_b->v[i], sizeof(mydouble));
        inFile.read((char*)&_fp12e->m_a->m_c->v[i], sizeof(mydouble));
        inFile.read((char*)&_fp12e->m_b->m_a->v[i], sizeof(mydouble));
        inFile.read((char*)&_fp12e->m_b->m_b->v[i], sizeof(mydouble));
        inFile.read((char*)&_fp12e->m_b->m_c->v[i], sizeof(mydouble));
    }
}

void GTDCLXVI::writeToFile(std::ostream& outFile) const {
    for(int i = 0; i < 24; i++) {
        outFile.write((char*)&_fp12e->m_a->m_a->v[i], sizeof(mydouble));
        outFile.write((char*)&_fp12e->m_a->m_b->v[i], sizeof(mydouble));
        outFile.write((char*)&_fp12e->m_a->m_c->v[i], sizeof(mydouble));
        outFile.write((char*)&_fp12e->m_b->m_a->v[i], sizeof(mydouble));
        outFile.write((char*)&_fp12e->m_b->m_b->v[i], sizeof(mydouble));
        outFile.write((char*)&_fp12e->m_b->m_c->v[i], sizeof(mydouble));
    }
}
