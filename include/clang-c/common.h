/*===-- clang-c/Compiler.h - Compiler Public C Interface-----------*- C -*-===*\
|*                                                                            *|
|*                     The LLVM Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This header provides common functionality utilized by multiple components  *|
|* of the libclang C API.                                                     *|
\*===----------------------------------------------------------------------===*/

#ifndef CLANG_C_COMMON_H
#define CLANG_C_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* MSVC DLL import/export. */
#ifdef _MSC_VER
  #ifdef _CLANGC_LIB_
    #define CLANGC_LINKAGE __declspec(dllexport)
  #else
    #define CLANGC_LINKAGE __declspec(dllimport)
  #endif
#else
  #define CLANGC_LINKAGE
#endif

/**
 * \defgroup CLANGC libclang: C Interface to Clang
 *
 * The C Interface to Clang makes available a subset of the Clang API to C.
 *
 * This interface will never provide all of the information representation
 * stored in Clang's C++ API, nor should it: the intent is to maintain an API
 * that is relatively stable from one release to the next, providing only the
 * basic functionality needed to support development tools.
 *
 * @{
 */

/**
 * \defgroup CINDEX_DIAG Diagnostic reporting
 *
 * @{
 */

/**
 * \brief Describes the severity of a particular diagnostic.
 */
enum CXDiagnosticSeverity {
  /**
   * \brief A diagnostic that has been suppressed, e.g., by a command-line
   * option.
   */
  CXDiagnostic_Ignored = 0,

  /**
   * \brief This diagnostic is a note that should be attached to the
   * previous (non-note) diagnostic.
   */
  CXDiagnostic_Note    = 1,

  /**
   * \brief This diagnostic indicates suspicious code that may not be
   * wrong.
   */
  CXDiagnostic_Warning = 2,

  /**
   * \brief This diagnostic indicates that the code is ill-formed.
   */
  CXDiagnostic_Error   = 3,

  /**
   * \brief This diagnostic indicates that the code is ill-formed such
   * that future parser recovery is unlikely to produce useful
   * results.
   */
  CXDiagnostic_Fatal   = 4
};

/**
 * \brief A single diagnostic, containing the diagnostic's severity,
 * location, text, source ranges, and fix-it hints.
 */
typedef void *CXDiagnostic;

/**
 * \brief A group of CXDiagnostics.
 */
typedef void *CXDiagnosticSet;

/**
 * \brief Determine the number of diagnostics in a CXDiagnosticSet.
 */
CINDEX_LINKAGE unsigned clang_getNumDiagnosticsInSet(CXDiagnosticSet Diags);

/**
 * \brief Retrieve a diagnostic associated with the given CXDiagnosticSet.
 *
 * \param Unit the CXDiagnosticSet to query.
 * \param Index the zero-based diagnostic number to retrieve.
 *
 * \returns the requested diagnostic. This diagnostic must be freed
 * via a call to \c clang_disposeDiagnostic().
 */
CINDEX_LINKAGE CXDiagnostic clang_getDiagnosticInSet(CXDiagnosticSet Diags,
                                                     unsigned Index);


/**
 * \brief Describes the kind of error that occurred (if any) in a call to
 * \c clang_loadDiagnostics.
 */
enum CXLoadDiag_Error {
  /**
   * \brief Indicates that no error occurred.
   */
  CXLoadDiag_None = 0,

  /**
   * \brief Indicates that an unknown error occurred while attempting to
   * deserialize diagnostics.
   */
  CXLoadDiag_Unknown = 1,

  /**
   * \brief Indicates that the file containing the serialized diagnostics
   * could not be opened.
   */
  CXLoadDiag_CannotLoad = 2,

  /**
   * \brief Indicates that the serialized diagnostics file is invalid or
   *  corrupt.
   */
  CXLoadDiag_InvalidFile = 3
};

/**
 * \brief Deserialize a set of diagnostics from a Clang diagnostics bitcode
 *  file.
 *
 * \param The name of the file to deserialize.
 * \param A pointer to a enum value recording if there was a problem
 *        deserializing the diagnostics.
 * \param A pointer to a CXString for recording the error string
 *        if the file was not successfully loaded.
 *
 * \returns A loaded CXDiagnosticSet if successful, and NULL otherwise.  These
 *  diagnostics should be released using clang_disposeDiagnosticSet().
 */
CINDEX_LINKAGE CXDiagnosticSet clang_loadDiagnostics(const char *file,
                                                  enum CXLoadDiag_Error *error,
                                                  CXString *errorString);

/**
 * \brief Release a CXDiagnosticSet and all of its contained diagnostics.
 */
CINDEX_LINKAGE void clang_disposeDiagnosticSet(CXDiagnosticSet Diags);

/**
 * \brief Retrieve the child diagnostics of a CXDiagnostic.  This
 *  CXDiagnosticSet does not need to be released by clang_diposeDiagnosticSet.
 */
CINDEX_LINKAGE CXDiagnosticSet clang_getChildDiagnostics(CXDiagnostic D);

/**
 * \brief Determine the number of diagnostics produced for the given
 * translation unit.
 */
CINDEX_LINKAGE unsigned clang_getNumDiagnostics(CXTranslationUnit Unit);

/**
 * \brief Retrieve a diagnostic associated with the given translation unit.
 *
 * \param Unit the translation unit to query.
 * \param Index the zero-based diagnostic number to retrieve.
 *
 * \returns the requested diagnostic. This diagnostic must be freed
 * via a call to \c clang_disposeDiagnostic().
 */
CINDEX_LINKAGE CXDiagnostic clang_getDiagnostic(CXTranslationUnit Unit,
                                                unsigned Index);

/**
 * \brief Retrieve the complete set of diagnostics associated with a
 *        translation unit.
 *
 * \param Unit the translation unit to query.
 */
CINDEX_LINKAGE CXDiagnosticSet
  clang_getDiagnosticSetFromTU(CXTranslationUnit Unit);

/**
 * \brief Destroy a diagnostic.
 */
CINDEX_LINKAGE void clang_disposeDiagnostic(CXDiagnostic Diagnostic);

/**
 * \brief Options to control the display of diagnostics.
 *
 * The values in this enum are meant to be combined to customize the
 * behavior of \c clang_displayDiagnostic().
 */
enum CXDiagnosticDisplayOptions {
  /**
   * \brief Display the source-location information where the
   * diagnostic was located.
   *
   * When set, diagnostics will be prefixed by the file, line, and
   * (optionally) column to which the diagnostic refers. For example,
   *
   * \code
   * test.c:28: warning: extra tokens at end of #endif directive
   * \endcode
   *
   * This option corresponds to the clang flag \c -fshow-source-location.
   */
  CXDiagnostic_DisplaySourceLocation = 0x01,

  /**
   * \brief If displaying the source-location information of the
   * diagnostic, also include the column number.
   *
   * This option corresponds to the clang flag \c -fshow-column.
   */
  CXDiagnostic_DisplayColumn = 0x02,

  /**
   * \brief If displaying the source-location information of the
   * diagnostic, also include information about source ranges in a
   * machine-parsable format.
   *
   * This option corresponds to the clang flag
   * \c -fdiagnostics-print-source-range-info.
   */
  CXDiagnostic_DisplaySourceRanges = 0x04,

  /**
   * \brief Display the option name associated with this diagnostic, if any.
   *
   * The option name displayed (e.g., -Wconversion) will be placed in brackets
   * after the diagnostic text. This option corresponds to the clang flag
   * \c -fdiagnostics-show-option.
   */
  CXDiagnostic_DisplayOption = 0x08,

  /**
   * \brief Display the category number associated with this diagnostic, if any.
   *
   * The category number is displayed within brackets after the diagnostic text.
   * This option corresponds to the clang flag
   * \c -fdiagnostics-show-category=id.
   */
  CXDiagnostic_DisplayCategoryId = 0x10,

  /**
   * \brief Display the category name associated with this diagnostic, if any.
   *
   * The category name is displayed within brackets after the diagnostic text.
   * This option corresponds to the clang flag
   * \c -fdiagnostics-show-category=name.
   */
  CXDiagnostic_DisplayCategoryName = 0x20
};

/**
 * \brief Format the given diagnostic in a manner that is suitable for display.
 *
 * This routine will format the given diagnostic to a string, rendering
 * the diagnostic according to the various options given. The
 * \c clang_defaultDiagnosticDisplayOptions() function returns the set of
 * options that most closely mimics the behavior of the clang compiler.
 *
 * \param Diagnostic The diagnostic to print.
 *
 * \param Options A set of options that control the diagnostic display,
 * created by combining \c CXDiagnosticDisplayOptions values.
 *
 * \returns A new string containing for formatted diagnostic.
 */
CINDEX_LINKAGE CXString clang_formatDiagnostic(CXDiagnostic Diagnostic,
                                               unsigned Options);

/**
 * \brief Retrieve the set of display options most similar to the
 * default behavior of the clang compiler.
 *
 * \returns A set of display options suitable for use with \c
 * clang_displayDiagnostic().
 */
CINDEX_LINKAGE unsigned clang_defaultDiagnosticDisplayOptions(void);

/**
 * \brief Determine the severity of the given diagnostic.
 */
CINDEX_LINKAGE enum CXDiagnosticSeverity
clang_getDiagnosticSeverity(CXDiagnostic);

/**
 * \brief Retrieve the source location of the given diagnostic.
 *
 * This location is where Clang would print the caret ('^') when
 * displaying the diagnostic on the command line.
 */
CINDEX_LINKAGE CXSourceLocation clang_getDiagnosticLocation(CXDiagnostic);

/**
 * \brief Retrieve the text of the given diagnostic.
 */
CINDEX_LINKAGE CXString clang_getDiagnosticSpelling(CXDiagnostic);

/**
 * \brief Retrieve the name of the command-line option that enabled this
 * diagnostic.
 *
 * \param Diag The diagnostic to be queried.
 *
 * \param Disable If non-NULL, will be set to the option that disables this
 * diagnostic (if any).
 *
 * \returns A string that contains the command-line option used to enable this
 * warning, such as "-Wconversion" or "-pedantic".
 */
CINDEX_LINKAGE CXString clang_getDiagnosticOption(CXDiagnostic Diag,
                                                  CXString *Disable);

/**
 * \brief Retrieve the category number for this diagnostic.
 *
 * Diagnostics can be categorized into groups along with other, related
 * diagnostics (e.g., diagnostics under the same warning flag). This routine
 * retrieves the category number for the given diagnostic.
 *
 * \returns The number of the category that contains this diagnostic, or zero
 * if this diagnostic is uncategorized.
 */
CINDEX_LINKAGE unsigned clang_getDiagnosticCategory(CXDiagnostic);

/**
 * \brief Retrieve the name of a particular diagnostic category.
 *
 * \param Category A diagnostic category number, as returned by
 * \c clang_getDiagnosticCategory().
 *
 * \returns The name of the given diagnostic category.
 */
CINDEX_LINKAGE CXString clang_getDiagnosticCategoryName(unsigned Category);

/**
 * \brief Determine the number of source ranges associated with the given
 * diagnostic.
 */
CINDEX_LINKAGE unsigned clang_getDiagnosticNumRanges(CXDiagnostic);

/**
 * \brief Retrieve a source range associated with the diagnostic.
 *
 * A diagnostic's source ranges highlight important elements in the source
 * code. On the command line, Clang displays source ranges by
 * underlining them with '~' characters.
 *
 * \param Diagnostic the diagnostic whose range is being extracted.
 *
 * \param Range the zero-based index specifying which range to
 *
 * \returns the requested source range.
 */
CINDEX_LINKAGE CXSourceRange clang_getDiagnosticRange(CXDiagnostic Diagnostic,
                                                      unsigned Range);

/**
 * \brief Determine the number of fix-it hints associated with the
 * given diagnostic.
 */
CINDEX_LINKAGE unsigned clang_getDiagnosticNumFixIts(CXDiagnostic Diagnostic);

/**
 * \brief Retrieve the replacement information for a given fix-it.
 *
 * Fix-its are described in terms of a source range whose contents
 * should be replaced by a string. This approach generalizes over
 * three kinds of operations: removal of source code (the range covers
 * the code to be removed and the replacement string is empty),
 * replacement of source code (the range covers the code to be
 * replaced and the replacement string provides the new code), and
 * insertion (both the start and end of the range point at the
 * insertion location, and the replacement string provides the text to
 * insert).
 *
 * \param Diagnostic The diagnostic whose fix-its are being queried.
 *
 * \param FixIt The zero-based index of the fix-it.
 *
 * \param ReplacementRange The source range whose contents will be
 * replaced with the returned replacement string. Note that source
 * ranges are half-open ranges [a, b), so the source code should be
 * replaced from a and up to (but not including) b.
 *
 * \returns A string containing text that should be replace the source
 * code indicated by the \c ReplacementRange.
 */
CINDEX_LINKAGE CXString clang_getDiagnosticFixIt(CXDiagnostic Diagnostic,
                                                 unsigned FixIt,
                                               CXSourceRange *ReplacementRange);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif