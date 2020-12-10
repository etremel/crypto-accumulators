/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#include <bilinear/G1_DCLXVI.hpp>
#include <utils/Pointers.hpp>

extern const scalar_t bn_n;
extern const curvepoint_fp_t bn_curvegen;

G1DCLXVI::G1DCLXVI() {
    curvepoint_fp_set(_curvepoint, bn_curvegen);
    fpe_isreduced(_curvepoint->m_x);
    fpe_isreduced(_curvepoint->m_y);
}

G1DCLXVI::G1DCLXVI(const G1DCLXVI& other) {
    curvepoint_fp_t eObj;
    other.exportObject(eObj);
    curvepoint_fp_set(_curvepoint, eObj);
}

G& G1DCLXVI::operator=(const G& other) {
    if(this == &other) {
        return *this;
    } else {
        G1DCLXVI* pG1 = (G1DCLXVI*)&other;
        curvepoint_fp_t eObj;
        pG1->exportObject(eObj);
        curvepoint_fp_set(_curvepoint, eObj);
        return *this;
    }
}

void G1DCLXVI::generateRandom() {
    scalar_t scalar;
    scalar_setrandom(scalar, bn_n);
    curvepoint_fp_scalarmult_vartime(_curvepoint, _curvepoint, scalar);
    curvepoint_fp_makeaffine(_curvepoint);
    isReduced(_curvepoint);
}

void G1DCLXVI::becomeIdentity() {
    curvepoint_fp_setneutral(_curvepoint);
}

void G1DCLXVI::becomeGenerator() {
    curvepoint_fp_set(_curvepoint, bn_curvegen);
    fpe_isreduced(_curvepoint->m_x);
    fpe_isreduced(_curvepoint->m_y);
}

bool G1DCLXVI::isEqual(const G& other) {
    const G1DCLXVI& otherG1 = ref_cast<G1DCLXVI>(other);
    curvepoint_fp_t op;
    otherG1.exportObject(op);
    curvepoint_fp_makeaffine(_curvepoint);
    curvepoint_fp_makeaffine(op);
    return (fpe_iseq(_curvepoint->m_x, op->m_x)
            && fpe_iseq(_curvepoint->m_y, op->m_y)
            && fpe_iseq(_curvepoint->m_z, op->m_z));
}

void G1DCLXVI::doMultiplication(const G& other, G& result) {
    const G1DCLXVI& otherG1 = ref_cast<G1DCLXVI>(other);
    G1DCLXVI& resultG1 = ref_cast<G1DCLXVI>(result);
    curvepoint_fp_add_vartime(resultG1.getUnderlyingObj(), _curvepoint, otherG1.getUnderlyingObj());
    curvepoint_fp_makeaffine(resultG1.getUnderlyingObj());
}

void G1DCLXVI::doPower(const Scalar& scalar, G& result) {
    const ScalarDCLXVI& dScalar = ref_cast<ScalarDCLXVI>(scalar);
    G1DCLXVI& resultG1 = ref_cast<G1DCLXVI>(result);
    curvepoint_fp_scalarmult_vartime(resultG1.getUnderlyingObj(), _curvepoint, dScalar.getUnderlyingObj());
    curvepoint_fp_makeaffine(resultG1.getUnderlyingObj());
}

void G1DCLXVI::importObject(const void* obj) {
    curvepoint_fp_struct_t* op = (curvepoint_fp_struct_t*)obj;
    curvepoint_fp_set(_curvepoint, op);
}

void G1DCLXVI::exportObject(void* obj) const {
    curvepoint_fp_struct_t* rop = (curvepoint_fp_struct_t*)obj;
    curvepoint_fp_set(rop, _curvepoint);
}

size_t G1DCLXVI::getSize() const {
    return (12 * (4 * sizeof(mydouble)));
}

char* G1DCLXVI::getByteBuffer() const {
    return (char*)_curvepoint;
}

curvepoint_fp_struct_t* G1DCLXVI::getUnderlyingObj() {
    return _curvepoint;
}

const curvepoint_fp_struct_t* G1DCLXVI::getUnderlyingObj() const {
    const curvepoint_fp_struct_t* const_curvepoint = _curvepoint;
    return const_curvepoint;
}

void G1DCLXVI::readFromFile(std::istream& inFile) {
    for(int i = 0; i < 12; i++) {
        inFile.read((char*)&_curvepoint->m_x->v[i], sizeof(mydouble));
        inFile.read((char*)&_curvepoint->m_y->v[i], sizeof(mydouble));
        inFile.read((char*)&_curvepoint->m_z->v[i], sizeof(mydouble));
        inFile.read((char*)&_curvepoint->m_t->v[i], sizeof(mydouble));
    }
}

void G1DCLXVI::writeToFile(std::ostream& outFile) const {
    for(int i = 0; i < 12; i++) {
        outFile.write((char*)&_curvepoint->m_x->v[i], sizeof(mydouble));
        outFile.write((char*)&_curvepoint->m_y->v[i], sizeof(mydouble));
        outFile.write((char*)&_curvepoint->m_z->v[i], sizeof(mydouble));
        outFile.write((char*)&_curvepoint->m_t->v[i], sizeof(mydouble));
    }
}

void G1DCLXVI::isReduced(curvepoint_fp_t curvepoint) {
    fpe_isreduced(curvepoint->m_x);
    fpe_isreduced(curvepoint->m_y);
    fpe_isreduced(curvepoint->m_z);
    fpe_isreduced(curvepoint->m_t);
}
