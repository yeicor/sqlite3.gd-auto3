#include "occ_register_types.hxx"
#include "OCCGeomCircle.hxx"

#include "OCCPoint.hxx"
#include "OCCEdge.hxx"
#include "OCCFace.hxx"
#include "OCCVertex.hxx"

#include "OCCWire.hxx"

#include "OCCCompound.hxx"

#include "OCCSolid.hxx"

#include "OCCShell.hxx"

#include "OCCCompSolid.hxx"

#include "OCCPnt.hxx"



#include "OCCShape.hxx"
#include "OCCVec.hxx"
#include "OCCDir.hxx"
#include "OCCAx1.hxx"
#include "OCCPnt2d.hxx"
#include "OCCLin.hxx"
#include "OCCAx2.hxx"
#include "OCCAx3.hxx"
#include "OCCCirc.hxx"
#include "OCCElips.hxx"
#include "OCCSphere.hxx"
#include "OCCCylinder.hxx"
#include "OCCParab.hxx"
#include "OCCTorus.hxx"
#include "OCCPln.hxx"
#include "OCCQuaternion.hxx"


#include "OCCMakeEdge.hxx"




#include "OCCBox.hxx"
#include "OCCSphereShape.hxx"

#include "OCCCylinderShape.hxx"
#include "OCCConeShape.hxx"

#include "OCCFuse.hxx"

#include "OCCCut.hxx"

#include "OCCCommon.hxx"

#include "OCCMeshExtractor.hxx"
#include "OCCSTLExporter.hxx"







#include <godot_cpp/core/class_db.hpp>

void occ_register_types() {
    GDREGISTER_CLASS(OCCEdge);
    GDREGISTER_CLASS(OCCFace);
    GDREGISTER_CLASS(OCCVertex);
    GDREGISTER_CLASS(OCCWire);
    GDREGISTER_CLASS(OCCCompound);
    GDREGISTER_CLASS(OCCSolid);
    GDREGISTER_CLASS(OCCShell);
    GDREGISTER_CLASS(OCCCompSolid);
    GDREGISTER_CLASS(OCCGeomCircle);
    GDREGISTER_CLASS(OCCPnt);
    GDREGISTER_CLASS(godot::OCCPoint);
    GDREGISTER_CLASS(OCCShape);
    GDREGISTER_CLASS(OCCMakeEdge);

    GDREGISTER_CLASS(OCCVec);
    GDREGISTER_CLASS(OCCDir);
    GDREGISTER_CLASS(OCCQuaternion);
    GDREGISTER_CLASS(OCCCommon);
    GDREGISTER_CLASS(OCCCut);
    GDREGISTER_CLASS(OCCFuse);
    GDREGISTER_CLASS(OCCConeShape);
    GDREGISTER_CLASS(OCCSTLExporter);
    GDREGISTER_CLASS(OCCMeshExtractor);

    GDREGISTER_CLASS(OCCCylinderShape);
    GDREGISTER_CLASS(OCCSphereShape);
    GDREGISTER_CLASS(OCCBox);
    GDREGISTER_CLASS(OCCPln);

    GDREGISTER_CLASS(OCCTorus);

    GDREGISTER_CLASS(OCCParab);

    GDREGISTER_CLASS(OCCCylinder);

    GDREGISTER_CLASS(OCCSphere);

    GDREGISTER_CLASS(OCCElips);

    GDREGISTER_CLASS(OCCCirc);

    GDREGISTER_CLASS(OCCAx1);
    GDREGISTER_CLASS(OCCPnt2d);
    GDREGISTER_CLASS(OCCLin);
    GDREGISTER_CLASS(OCCAx2);
    GDREGISTER_CLASS(OCCAx3);
}

void occ_unregister_types() {
    // Nothing to do yet
}
