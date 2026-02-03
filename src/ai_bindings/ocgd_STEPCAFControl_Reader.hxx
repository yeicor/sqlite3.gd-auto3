#ifndef _ocgd_STEPCAFControl_Reader_HeaderFile
#define _ocgd_STEPCAFControl_Reader_HeaderFile

/**
 * ocgd_STEPCAFControl_Reader.hxx
 *
 * Godot GDExtension wrapper for OpenCASCADE STEPCAFControl_Reader class.
 *
 * Provides a tool to read STEP file and put it into DECAF document. 
 * Besides transfer of shapes (including assemblies) provided by STEPControl, 
 * supports also colors and part names.
 *
 * This reader supports reading files with external references
 * i.e. multifile reading
 * It behaves as usual Reader (from STEPControl) for the main
 * file (e.g. if it is single file)
 * Results of reading other files can be accessed by name of the
 * file or by iterating on a readers
 *
 * Original OCCT header: <opencascade/STEPCAFControl_Reader.hxx>
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

#include <opencascade/STEPCAFControl_Reader.hxx>
#include <opencascade/Standard_Handle.hxx>
#include <opencascade/TDocStd_Document.hxx>
#include <opencascade/XSControl_WorkSession.hxx>
#include <opencascade/IFSelect_ReturnStatus.hxx>
#include <opencascade/Message_ProgressRange.hxx>
#include <opencascade/StepData_Factors.hxx>
#include <opencascade/XCAFDoc_DocumentTool.hxx>
#include <opencascade/XCAFDoc_ShapeTool.hxx>
#include <opencascade/TDF_LabelSequence.hxx>
#include <opencascade/TDataStd_Name.hxx>
#include <opencascade/TCollection_ExtendedString.hxx>

#include "ocgd_TopoDS_Shape.hxx"

using namespace godot;

/**
 * ocgd_STEPCAFControl_Reader
 *
 * Wrapper class to expose the OpenCASCADE STEPCAFControl_Reader to Godot.
 *
 * Provides a tool to read STEP file and put it into DECAF document. 
 * Besides transfer of shapes (including assemblies) provided by STEPControl, 
 * supports also colors and part names.
 *
 * This reader supports reading files with external references
 * i.e. multifile reading
 * It behaves as usual Reader (from STEPControl) for the main
 * file (e.g. if it is single file)
 * Results of reading other files can be accessed by name of the
 * file or by iterating on a readers
 */
class ocgd_STEPCAFControl_Reader : public RefCounted {
    GDCLASS(ocgd_STEPCAFControl_Reader, RefCounted);

protected:
    static void _bind_methods();

private:
    STEPCAFControl_Reader* _reader;
    bool _owns_reader;
    Handle(TDocStd_Document) _document;

public:
    //! Creates a reader with an empty
    //! STEP model and sets ColorMode, LayerMode, NameMode and
    //! PropsMode to Standard_True.
    ocgd_STEPCAFControl_Reader();

    //! Destructor.
    virtual ~ocgd_STEPCAFControl_Reader();

    //! Clears the internal data structures and attaches to a new session
    //! Clears the session if it was not yet set for STEP
    void init();

    //! Loads a file and returns the read status
    //! Provided for use like single-file reader.
    //! @param[in] theFileName  file to open
    //! @return read status (0=OK, >0=error)
    int read_file(const String& filename);

    //! Loads a file from stream and returns the read status.
    //! @param[in] theName  auxiliary stream name
    //! @param[in] theIStream  stream to read from
    //! @return read status (0=OK, >0=error)
    int read_stream(const String& name, const PackedByteArray& data);

    //! Returns number of roots recognized for transfer
    //! Shortcut for Reader().NbRootsForTransfer()
    int nb_roots_for_transfer();

    //! Translates currently loaded STEP file into the document
    //! Returns the transferred shape, or null if failed
    //! Provided for use like single-file reader
    Ref<ocgd_TopoDS_Shape> transfer_one_root(int num);

    //! Translates currently loaded STEP file into the document
    //! Returns the main assembly shape, or null if failed
    //! Provided for use like single-file reader
    Ref<ocgd_TopoDS_Shape> transfer();

    //! Translate STEP file given by filename into the document
    //! Returns the main assembly shape, or null if failed
    Ref<ocgd_TopoDS_Shape> perform(const String& filename);

    //! Get all root shapes from the loaded document
    Array get_root_shapes() const;

    //! Get the main document label
    String get_main_label() const;

    //! Set ColorMode for indicate read Colors or not.
    void set_color_mode(bool colormode);

    bool get_color_mode() const;

    //! Set NameMode for indicate read Name or not.
    void set_name_mode(bool namemode);

    bool get_name_mode() const;

    //! Set LayerMode for indicate read Layers or not.
    void set_layer_mode(bool layermode);

    bool get_layer_mode() const;

    //! PropsMode for indicate read Validation properties or not.
    void set_props_mode(bool propsmode);

    bool get_props_mode() const;

    //! MetaMode for indicate read Metadata or not.
    void set_meta_mode(bool metamode);

    bool get_meta_mode() const;

    //! MetaMode for indicate whether to read Product Metadata or not.
    void set_product_meta_mode(bool productmetamode);

    bool get_product_meta_mode() const;

    //! Set SHUO mode for indicate write SHUO or not.
    void set_shuo_mode(bool shuomode);

    bool get_shuo_mode() const;

    //! Set GDT mode for indicate write GDT or not.
    void set_gdt_mode(bool gdtmode);

    bool get_gdt_mode() const;

    //! Set Material mode
    void set_mat_mode(bool matmode);

    bool get_mat_mode() const;

    //! Set View mode
    void set_view_mode(bool viewmode);

    //! Get View mode
    bool get_view_mode() const;

    // Internal access to the wrapped OpenCASCADE object
    STEPCAFControl_Reader* get_occt_reader() const { return _reader; }
};

#endif // _ocgd_STEPCAFControl_Reader_HeaderFile