#include <igl/readOFF.h>
#include <igl/viewer/Viewer.h>
#include <igl/per_vertex_normals.h>
#include <igl/avg_edge_length.h>
#include <igl/embree/ambient_occlusion.h>
#include <iostream>

using namespace Eigen;
using namespace std;

// Mesh
Eigen::MatrixXd V;
Eigen::MatrixXi F;

VectorXd AO;

  // It allows to change the degree of the field when a number is pressed
bool key_down(igl::Viewer& viewer, unsigned char key, int modifier)
{
  const RowVector3d color(0.9,0.85,0.9);
  switch(key)
  {
    case '1':
      // Show the mesh without the ambient occlusion factor
      viewer.set_colors(color);
      break;
    case '2':
    {
      // Show the mesh with the ambient occlusion factor
      MatrixXd C = color.replicate(V.rows(),1);
      for (unsigned i=0; i<C.rows();++i)
        C.row(i) *= AO(i);//std::min<double>(AO(i)+0.2,1);
      viewer.set_colors(C);
      break;
    }
    case '.':
      viewer.options.lighting_factor += 0.1;
      break;
    case ',':
      viewer.options.lighting_factor -= 0.1;
      break;
    default: break;
  }
  viewer.options.lighting_factor = 
    std::min(std::max(viewer.options.lighting_factor,0.f),1.f);

  return false;
}

int main(int argc, char *argv[])
{
  using namespace std;
  using namespace Eigen;
  cout<<
    "Press 1 to turn off Ambient Occlusion"<<endl<<
    "Press 2 to turn on Ambient Occlusion"<<endl<<
    "Press . to turn up lighting"<<endl<<
    "Press , to turn down lighting"<<endl;

  // Load a mesh in OFF format
  igl::readOFF("../shared/fertility.off", V, F);

  MatrixXd N;
  igl::per_vertex_normals(V,F,N);

  // Compute ambient occlusion factor using embree
  igl::ambient_occlusion(V,F,V,N,500,AO);
  AO = 1.0 - AO.array();

  // Show mesh
  igl::Viewer viewer;
  viewer.set_mesh(V, F);
  viewer.callback_key_down = &key_down;
  key_down(viewer,'2',0);
  viewer.options.show_lines = false;
  viewer.options.lighting_factor = 0.0f;
  viewer.launch();
}
