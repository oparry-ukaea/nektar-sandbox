///////////////////////////////////////////////////////////////////////////////
//
// File: main.cpp
//
//
// Description: Entrypoint for MySolver.
//
///////////////////////////////////////////////////////////////////////////////
#include <LibUtilities/BasicUtils/SessionReader.h>
#include <SolverUtils/Driver.h>
#include <iostream>
#include <mpi.h>

namespace LU = Nektar::LibUtilities;
namespace SD = Nektar::SpatialDomains;
namespace SU = Nektar::SolverUtils;

int main(int argc, char *argv[]) {

  // Init MPI - gives us control over init/finalise rather than Nektar
  int provided_thread_level;
  if (MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED,
                      &provided_thread_level) != MPI_SUCCESS) {
    std::cout << "ERROR: MPI_Init != MPI_SUCCESS" << std::endl;
    return -1;
  }

  // Create session reader.
  auto session = LU::SessionReader::CreateInstance(argc, argv);

  // Read the mesh and create a MeshGraph object.
  auto graph = SD::MeshGraph::Read(session);

  // Create driver.
  std::string driverName;
  session->LoadSolverInfo("Driver", driverName, "Standard");
  auto drv = SU::GetDriverFactory().CreateInstance(driverName, session, graph);

  // Execute driver
  drv->Execute();

  // Finalise session
  session->Finalise();

  // Finalise MPI
  if (MPI_Finalize() != MPI_SUCCESS) {
    std::cout << "ERROR: MPI_Finalize != MPI_SUCCESS" << std::endl;
    return -1;
  }

  return 0;
}