/*
 * Shapeworks license
 */

/**
 * @file ShapeWorksView2.h
 * @brief Main application
 *
 */

#include <QMainWindow>
#include <QTimer>

#include <vtkSmartPointer.h>

#include "tinyxml.h"

#include "itkParticleShapeLinearRegressionMatrixAttribute.h"
#include "itkParticlePositionReader.h"
#include "itkParticleShapeStatistics.h"
#include "itkParticlePositionWriter.h"

#include <ColorSchemes.h>
#include <MeshManager.h>
#include <Preferences.h>
#include <PreferencesWindow.h>

class vtkRenderer;
class vtkLookupTable;
class vtkColorTransferFunction;
class vtkPoints;
class vtkPolyData;
class vtkGlyph3D;
class vtkPolyDataMapper;
class vtkActor;
class vtkSphereSource;
class vtkArrowSource;
class vtkTransform;
class vtkFloatArray;
class vtkOrientationMarkerWidget;
class vtkImageData;
class vtkTransformPolyDataFilter;
//class vtkScalarBarActor;

// Forward Qt class declarations
class Ui_ShapeWorksView2;

class ViewerLayout;

class ShapeWorksView2 : public QMainWindow
{
  Q_OBJECT
public:

  ShapeWorksView2( int argc, char** argv );
  ~ShapeWorksView2();

  void write_stats_file(QString filename);

  virtual void closeEvent( QCloseEvent* event );

public Q_SLOTS:

  void on_actionExportPcaLoadings_triggered();
  void on_actionExportPoints_triggered();
  void on_actionExportSurfaceMesh_triggered();
  void on_actionExportVarianceReport_triggered();
  void on_actionExportEigenvectors_triggered();

  void on_actionImportScalarValues_triggered();
  void on_actionLoadScalars_triggered();

  void on_actionQuit_triggered();
  void on_actionPreferences_triggered();

  // analysis mode
  void on_tabWidget_currentChanged();

  // mean mode
  void on_meanOverallButton_clicked();
  void on_meanGroup1Button_clicked();
  void on_meanGroup2Button_clicked();
  void on_meanDifferenceButton_clicked();

  // sample mode
  void on_sampleSpinBox_valueChanged();
  void on_medianButton_clicked();
  void on_medianGroup1Button_clicked();
  void on_medianGroup2Button_clicked();

  // PCA mode
  void on_pcaSlider_valueChanged();
  void on_pcaModeSpinBox_valueChanged();
  void on_pcaGroupSlider_valueChanged();
  void on_pcaAnimateCheckBox_stateChanged();
  void on_pcaGroupAnimateCheckBox_stateChanged();
  void handlePcaTimer();
  void handleGroupTimer();

  // Regression mode
  void on_regressionSlider_valueChanged();
  void on_regressionAnimateCheckBox_stateChanged();
  void handleRegressionTimer();

  // visualization
  void on_showGlyphs_stateChanged();
  void on_showSurface_stateChanged();
  void on_neighborhoodSpinBox_valueChanged();
  void on_spacingSpinBox_valueChanged();
  void on_smoothingSlider_valueChanged();

  void colorSchemeChanged();
  void glyphPropertiesChanged();
  void handleSliderPreferencesChanged();
  void handle_clear_cache();
  void handle_redraw();
  void handle_pca();

private:

  void initializeRenderer();
  void initializeGlyphs();
  void initializeSurfaces();

  void updateAnalysisMode();
  void updateSurfaceSettings();
  void updateActors();
  void updateColorScheme();
  void updateGlyphProperties();
  void updateSliders();
  void updateDifferenceLUT( float r0, float r1 );

  void redraw();

  bool readParameterFile( char* filename );
  bool readExplanatoryVariables( char* filename );

  void displayShape( const vnl_vector<double> &pos );
  void displayVectorField( const std::vector<itk::ParticlePositionReader<3>::PointType > &vecs );
  void computePointDifferences( const std::vector<itk::ParticlePositionReader<3>::PointType > &vecs,
                                vtkSmartPointer<vtkFloatArray> magnitudes, vtkSmartPointer<vtkFloatArray> vectors );
  void computeSurfaceDifferences( vtkSmartPointer<vtkFloatArray> magnitudes,
                                  vtkSmartPointer<vtkFloatArray> vectors );

  void displayMeanDifference();
  void displayScalars();
  void displaySpheres();
  void resetPointScalars();

  void computeModeShape();
  void computeRegressionShape();

  double getRegressionValue( int sliderValue );
  double getPcaValue( int sliderValue );

  void trilinearInterpolate( vtkImageData* grad, double x, double y, double z,
                             vnl_vector_fixed<double, 3> &ans ) const;

  vnl_vector<double> getDomainShape( const vnl_vector<double> &shape, int domain );

  vtkSmartPointer<vtkPolyData> getDomainPoints( int domain );

  void setPregenSteps();

  vtkSmartPointer<vtkOrientationMarkerWidget> createOrientationMarker();

  // designer form
  Ui_ShapeWorksView2* ui;
  Preferences prefs_;

  vtkSmartPointer<vtkRenderer>             renderer;
  vtkSmartPointer<vtkOrientationMarkerWidget> orientationMarkerWidget;
  vtkSmartPointer<vtkLookupTable>          lut;
  vtkSmartPointer<vtkPoints>               glyphPoints;
  vtkSmartPointer<vtkPolyData>             glyphPointSet;
  vtkSmartPointer<vtkGlyph3D>              glyphs;
  vtkSmartPointer<vtkPolyDataMapper>       glyphMapper;
  vtkSmartPointer<vtkActor>                glyphActor;
  vtkSmartPointer<vtkSphereSource>         sphereSource;

  std::vector< vtkSmartPointer<vtkPolyDataMapper> >      surfaceMappers;
  std::vector< vtkSmartPointer<vtkActor> >               surfaceActors;

  vtkSmartPointer<vtkColorTransferFunction>   differenceLUT;
  vtkSmartPointer<vtkArrowSource>             arrowSource;
  vtkSmartPointer<vtkTransformPolyDataFilter> arrowFlipFilter;
  vtkSmartPointer<vtkGlyph3D>                 arrowGlyphs;
  vtkSmartPointer<vtkPolyDataMapper>          arrowGlyphMapper;
  vtkSmartPointer<vtkActor>                   arrowGlyphActor;

  vtkSmartPointer<vtkTransform>               transform180;
  //vtkSmartPointer<vtkColorTransferFunction>   pValueTFunc;
  //vtkSmartPointer<vtkPolyDataNormals>         m_surfNormals;
  //vtkSmartPointer<vtkDecimatePro>             m_surfDecimate;

  ParticleShapeStatistics<3> stats;
  itk::ParticleShapeLinearRegressionMatrixAttribute<double, 3>::Pointer regression;


  QList<double> scalars;

  int numSamples;
  int numDomains;
  int numPoints;
  int pointsPerDomain;

  bool groupsAvailable;
  bool regressionAvailable;

  double regressionMin;
  double regressionMax;
  double regressionRange;

  // a copy of the current shape mesh
  vnl_vector<double> currentShape;

  // step values for pregenerating meshes
  std::vector< int > pregenSteps;

  bool arrowsVisible;

  MeshManager meshManager;

  ColorSchemes m_ColorSchemes;

  bool pcaAnimateDirection;
  QTimer pcaAnimateTimer;

  bool regressionAnimateDirection;
  QTimer regressionAnimateTimer;

  bool groupAnimateDirection;
  QTimer groupAnimateTimer;

  std::vector<float> scalarValues;
  PreferencesWindow pref_window_;

//  vtkSmartPointer<vtkScalarBarActor> scalar_bar_actor_;

};
