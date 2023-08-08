///////////////////////////////////////////////////////////////////////////////
//
// File: main.cpp
//
// Description: 3D Helmholtz solver demo; mostly copied from Helmholtz3D.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstdlib>

#include <LibUtilities/BasicUtils/SessionReader.h>
#include <LibUtilities/Communication/Comm.h>
#include <LibUtilities/Memory/NekMemoryManager.hpp>
#include <MultiRegions/ContField.h>
#include <SpatialDomains/MeshGraph.h>

using namespace std;
using namespace Nektar;

namespace LU = Nektar::LibUtilities;
namespace SD = Nektar::SpatialDomains;

int main(int argc, char *argv[]) {
  LU::SessionReaderSharedPtr vSession =
      LU::SessionReader::CreateInstance(argc, argv);

  LibUtilities::CommSharedPtr vComm = vSession->GetComm();
  MultiRegions::ContFieldSharedPtr Exp, Fce;
  MultiRegions::ExpListSharedPtr DerExp1, DerExp2, DerExp3;
  int i, nq, coordim;
  Array<OneD, NekDouble> fce;
  Array<OneD, NekDouble> xc0, xc1, xc2;
  StdRegions::ConstFactorMap factors;
  StdRegions::VarCoeffMap varcoeffs;

  if (argc < 2) {
    fprintf(stderr, "Usage: Helmholtz3D  meshfile [solntype]\n");
    exit(1);
  }

  try {
    LU::FieldIOSharedPtr fld = LU::FieldIO::CreateDefault(vSession);

    //----------------------------------------------
    // Read in mesh from input file
    SD::MeshGraphSharedPtr graph3D = SD::MeshGraph::Read(vSession);
    //----------------------------------------------

    //----------------------------------------------
    // Define Expansion
    Exp = MemoryManager<MultiRegions::ContField>::AllocateSharedPtr(
        vSession, graph3D, vSession->GetVariable(0));
    //----------------------------------------------

    //----------------------------------------------
    // Set up coordinates of mesh for function evaluation
    coordim = Exp->GetCoordim(0);
    nq = Exp->GetTotPoints();

    xc0 = Array<OneD, NekDouble>(nq, 0.0);
    xc1 = Array<OneD, NekDouble>(nq, 0.0);
    xc2 = Array<OneD, NekDouble>(nq, 0.0);

    switch (coordim) {
    case 3:
      Exp->GetCoords(xc0, xc1, xc2);
      break;
    default:
      ASSERTL0(false, "Coordim not valid");
      break;
    }
    //----------------------------------------------

    //----------------------------------------------
    // Set factors or variable coefficients if they were defined as parameters
    // or functions (Lambda is required)
    factors[StdRegions::eFactorLambda] = vSession->GetParameter("Lambda");

    if (vSession->DefinesParameter("d00")) {
      NekDouble d00;
      vSession->LoadParameter("d00", d00, 1.0);
      factors[StdRegions::eFactorCoeffD00] = d00;
    } else if (vSession->DefinesFunction("d00")) {
      Array<OneD, NekDouble> d00(nq, 0.0);
      LibUtilities::EquationSharedPtr d00func = vSession->GetFunction("d00", 0);
      d00func->Evaluate(xc0, xc1, xc2, d00);
      varcoeffs[StdRegions::eVarCoeffD00] = d00;
    }

    if (vSession->DefinesParameter("d01")) {
      NekDouble d01;
      vSession->LoadParameter("d01", d01, 1.0);
      factors[StdRegions::eFactorCoeffD01] = d01;
    } else if (vSession->DefinesFunction("d01")) {
      Array<OneD, NekDouble> d01(nq, 0.0);
      LibUtilities::EquationSharedPtr d01func = vSession->GetFunction("d01", 0);
      d01func->Evaluate(xc0, xc1, xc2, d01);
      varcoeffs[StdRegions::eVarCoeffD01] = d01;
    }

    if (vSession->DefinesParameter("d11")) {
      NekDouble d11;
      vSession->LoadParameter("d11", d11, 1.0);
      factors[StdRegions::eFactorCoeffD11] = d11;
    } else if (vSession->DefinesFunction("d11")) {
      Array<OneD, NekDouble> d11(nq, 0.0);
      LibUtilities::EquationSharedPtr d11func = vSession->GetFunction("d11", 0);
      d11func->Evaluate(xc0, xc1, xc2, d11);
      varcoeffs[StdRegions::eVarCoeffD11] = d11;
    }

    if (vSession->DefinesParameter("d02")) {
      NekDouble d02;
      vSession->LoadParameter("d02", d02, 1.0);
      factors[StdRegions::eFactorCoeffD02] = d02;
    } else if (vSession->DefinesFunction("d02")) {
      Array<OneD, NekDouble> d02(nq, 0.0);
      LibUtilities::EquationSharedPtr d02func = vSession->GetFunction("d02", 0);
      d02func->Evaluate(xc0, xc1, xc2, d02);
      varcoeffs[StdRegions::eVarCoeffD02] = d02;
    }

    if (vSession->DefinesParameter("d12")) {
      NekDouble d12;
      vSession->LoadParameter("d12", d12, 1.0);
      factors[StdRegions::eFactorCoeffD12] = d12;
    } else if (vSession->DefinesFunction("d12")) {
      Array<OneD, NekDouble> d12(nq, 0.0);
      LibUtilities::EquationSharedPtr d12func = vSession->GetFunction("d12", 0);
      d12func->Evaluate(xc0, xc1, xc2, d12);
      varcoeffs[StdRegions::eVarCoeffD12] = d12;
    }

    if (vSession->DefinesParameter("d22")) {
      NekDouble d22;
      vSession->LoadParameter("d22", d22, 1.0);
      factors[StdRegions::eFactorCoeffD22] = d22;
    } else if (vSession->DefinesFunction("d22")) {
      Array<OneD, NekDouble> d22(nq, 0.0);
      LibUtilities::EquationSharedPtr d22func = vSession->GetFunction("d22", 0);
      d22func->Evaluate(xc0, xc1, xc2, d22);
      varcoeffs[StdRegions::eVarCoeffD22] = d22;
    }

    if (vSession->DefinesParameter("fn_vardiff")) {
      NekDouble tau;
      vSession->LoadParameter("fn_vardiff", tau, 1.0);
      if (tau > 0) {
        factors[StdRegions::eFactorTau] = 1.0;
      }
    }
    //----------------------------------------------

    // Retrieve basis key
    const SD::ExpansionInfoMap &expansions = graph3D->GetExpansionInfo();
    LU::BasisKey bkey0 = expansions.begin()->second->m_basisKeyVector[0];

    // Print summary of solution details
    if (vSession->GetComm()->GetRank() == 0) {
      cout << "Solving 3D Helmholtz:" << endl;
      cout << "  - Communication: " << vSession->GetComm()->GetType() << " ("
           << vSession->GetComm()->GetSize() << " processes)" << endl;
      cout << "  - Solver type  : " << vSession->GetSolverInfo("GlobalSysSoln")
           << endl;
      cout << "  - No. modes    : " << bkey0.GetNumModes() << endl;
      cout << "  - Helmsolve params:" << endl;
      cout << "    - Lambda : " << factors[StdRegions::eFactorLambda] << endl;

      for (auto &coeff : {"d00", "d11", "d22"}) {
        cout << "    - " << coeff << "    : ";
        if (vSession->DefinesParameter(coeff)) {
          cout << vSession->GetParameter(coeff);
        } else if (vSession->DefinesFunction(coeff) &&
                   !vSession->DefinesParameter("skip_varcoeffs")) {
          cout << "Defined via function";
        } else {
          cout << "Not set";
        }
        cout << endl;
      }
      cout << endl;
    }
    //----------------------------------------------

    //----------------------------------------------
    // Define forcing function for first variable defined in file
    fce = Array<OneD, NekDouble>(nq);
    LibUtilities::EquationSharedPtr ffunc = vSession->GetFunction("Forcing", 0);

    ffunc->Evaluate(xc0, xc1, xc2, fce);
    //----------------------------------------------

    //----------------------------------------------
    // Setup expansion containing the  forcing function
    Fce = MemoryManager<MultiRegions::ContField>::AllocateSharedPtr(*Exp);
    Fce->SetPhys(fce);
    //----------------------------------------------

    //----------------------------------------------
    // Helmholtz solution taking physical forcing after setting
    // initial condition to zero
    Vmath::Zero(Exp->GetNcoeffs(), Exp->UpdateCoeffs(), 1);
    if (vSession->DefinesParameter("skip_varcoeffs")) {
      if (vSession->GetComm()->GetRank() == 0) {
        cout << "***Ignoring varcoeffs in Helmsolve***" << endl;
      }
      Exp->HelmSolve(Fce->GetPhys(), Exp->UpdateCoeffs(), factors);
    } else {
      Exp->HelmSolve(Fce->GetPhys(), Exp->UpdateCoeffs(), factors, varcoeffs);
    }
    //----------------------------------------------

    //----------------------------------------------
    // Backward Transform Solution to get solved values at
    Exp->BwdTrans(Exp->GetCoeffs(), Exp->UpdatePhys());
    //----------------------------------------------

    //----------------------------------------------
    // See if there is an exact solution, if so
    // evaluate and plot errors
    LibUtilities::EquationSharedPtr ex_sol =
        vSession->GetFunction("ExactSolution", 0);

    //-----------------------------------------------
    // Write solution to file
    string out = vSession->GetSessionName() + ".fld";
    std::vector<LibUtilities::FieldDefinitionsSharedPtr> FieldDef =
        Exp->GetFieldDefinitions();
    std::vector<std::vector<NekDouble>> FieldData(FieldDef.size());

    for (i = 0; i < FieldDef.size(); ++i) {
      FieldDef[i]->m_fields.push_back("u");
      Exp->AppendFieldData(FieldDef[i], FieldData[i]);
    }
    fld->Write(out, FieldDef, FieldData);
    //-----------------------------------------------

    if (ex_sol) {
      //----------------------------------------------
      // evaluate exact solution
      ex_sol->Evaluate(xc0, xc1, xc2, fce);

      //----------------------------------------------

      //--------------------------------------------
      // Calculate L_inf error
      Fce->SetPhys(fce);
      Fce->SetPhysState(true);

      //--------------------------------------------
      // Calculate errors
      NekDouble vLinfError = Exp->Linf(Exp->GetPhys(), Fce->GetPhys());
      NekDouble vL2Error = Exp->L2(Exp->GetPhys(), Fce->GetPhys());
      NekDouble vH1Error = Exp->H1(Exp->GetPhys(), Fce->GetPhys());
      if (vComm->GetRank() == 0) {
        cout << "L infinity error: " << vLinfError << endl;
        cout << "L 2 error:        " << vL2Error << endl;
        cout << "H 1 error:        " << vH1Error << endl;
      }
      //--------------------------------------------
    }
    //----------------------------------------------
  } catch (const std::runtime_error &) {
    cout << "Caught an error" << endl;
    return 1;
  }

  vComm->Finalise();

  return 0;
}
