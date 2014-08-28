#ifndef R1H_OBJLOADER_H
#define R1H_OBJLOADER_H

#include <string>
#include <map>

namespace r1h {

class WavefrontObj {
    std::string basepath;
    std::string filepath;
    
public:
    enum ParameterType {
        // obj
        OBJ_unknown = 0,
        OBJ_v       = 'v',
        OBJ_vn      = 'vn',
        OBJ_vt      = 'vt',
        OBJ_o       = 'o',
        OBJ_g       = 'g',
        OBJ_s       = 's',
        OBJ_f       = 'f',
        OBJ_mtllib  = 'mlib',
        OBJ_usemtl  = 'usem',
        // matl
        MTL_newmtl  = 'newm',
        MTL_Ns      = 'Ns',
        MTL_Ka      = 'Ka',
        MTL_Kd      = 'Kd',
        MTL_Ks      = 'Ks',
        MTL_Ni      = 'Ni',
        MTL_d       = 'd',
        MTL_illum   = 'ilm',
        MTL_map_Kd  = 'mpKd'
    };
    
    enum ValueType {
        kNoneValue,
        kVectorValue,
        kStringValue,
        kFaceValue,
        kIntegerValue
    };
    
    struct ParameterDesc {
        ParameterType paramType;
        ValueType valueType;
        ParameterDesc(): paramType(OBJ_unknown), valueType(kNoneValue) {};
        ParameterDesc(const ParameterType pt, const ValueType vt): paramType(pt), valueType(vt) {};
    };
    
    
private:
    std::map<std::string, ParameterDesc> ptMap;
    void loadFile(const char *fpath);
    
public:
    ////
    struct FaceInfo {
        int v, vt, vn;
    };
    
    WavefrontObj(const char *path);
    virtual ~WavefrontObj(){}
    
    virtual void setBasePath(const char *path) {
        basepath = path;
        if(basepath[basepath.length() - 1] != '/') {
            basepath.push_back('/');
        }
    }
    virtual void load();
    
    // implement below
    // v, vn, vt, Ns, Ka, Kd, Ks, Ni, d
    virtual void foundVector(const ParameterType pt, const double x, const double y, const double z) = 0;
    // mtllib, o, s, usemtl, newmtl, MTL_map_Kd
    virtual void foundString(const ParameterType pt, const std::string &str) = 0;
    // illum
    virtual void foundInteger(const ParameterType pt, const int i) = 0;
    // f
    virtual void foundFace(const ParameterType pt, const std::vector<FaceInfo> &fids) = 0;
    
    // mtl
    virtual void importMTL(const char *filename); // optional
    
    // endof .mtl or .obj
    virtual void endFile() = 0;
    
    void addParameterDesc(const std::string &key, const ParameterDesc &desc) {
        ptMap[key] = desc;
    }
};

}
#endif