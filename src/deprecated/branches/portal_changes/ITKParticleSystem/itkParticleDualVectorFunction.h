/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleDualVectorFunction.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleDualVectorFunction_h
#define __itkParticleDualVectorFunction_h

#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include "itkWeakPointer.h"
#include "itkParticleSystem.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk
{

/**
 * \class ParticleDualVectorFunction
 *
 * This class combines the results of evaluating 2 ParticleVector functions and
 * presents the interface of a single function evaluation. Optionally, only the
 * first function can be used by calling SetLinkOff().
 *
 */
template <unsigned int VDimension>
class ParticleDualVectorFunction : public ParticleVectorFunction<VDimension>
{
public:
 /** Standard class typedefs. */
  typedef ParticleDualVectorFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef ParticleVectorFunction<VDimension> Superclass;
  itkTypeMacro( ParticleDualVectorFunction, ParticleVectorFunction);

  /** Type of particle system. */
  typedef typename Superclass::ParticleSystemType ParticleSystemType;

  /** Vector type. */
  typedef typename Superclass::VectorType VectorType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** The first argument is a pointer to the particle system.  The second
      argument is the index of the domain within that particle system.  The
      third argument is the index of the particle location within the given
      domain. */
  virtual VectorType Evaluate(unsigned int idx, unsigned int d,
                              const ParticleSystemType *system,
                              double &maxmove) const
  {
    double maxA, maxB, maxC;
    VectorType ansA;
    ansA.fill(0.0);
    const_cast<ParticleDualVectorFunction *>(this)->m_Counter = m_Counter + 1.0;
    if (m_AOn == true)
      {
      ansA = m_FunctionA->Evaluate(idx, d, system, maxA);
      const_cast<ParticleDualVectorFunction *>(this)->m_AverageGradMagA = m_AverageGradMagA + ansA.magnitude();
      }
    
    if (m_BOn == true)
      {
      VectorType ansB = m_FunctionB->Evaluate(idx, d, system, maxB);
      const_cast<ParticleDualVectorFunction *>(this)->m_AverageGradMagB = m_AverageGradMagB + ansB.magnitude();

      VectorType ansC = m_FunctionC->Evaluate(idx, d, system, maxC);
      const_cast<ParticleDualVectorFunction *>(this)->m_AverageGradMagC = m_AverageGradMagC + ansC.magnitude();
      
      if (m_AOn == true)  // both A and B are active
        {
        if (maxA > maxB) { maxmove = maxB + m_RelativeNormEnergyScaling * maxC; }
        else { maxmove = maxA; }
        return (ansA + m_RelativeGradientScaling * ansB + m_RelativeNormGradientScaling * ansC);
        }
      else // only B is active
        {
        maxmove = maxB + m_RelativeNormEnergyScaling * maxC;
        return ansB + ansC;
        }
      }
    else  // only A is active
      {
      maxmove = maxA;
      return ansA;
      }
    
    // If nothing is turned on, then return a max time
    // step of 0 and a bogus vector.
    maxmove = 0.0;
    return ansA;
  }

  virtual double Energy(unsigned int idx, unsigned int d, const ParticleSystemType *system) const
  {
    double ansA = 0.0;
    if (m_AOn == true)
      {
      ansA = m_FunctionA->Energy(idx, d, system);
      }

    if (m_BOn == true)
      {
      double ansB = m_FunctionB->Energy(idx, d, system);
      double ansC = m_FunctionC->Energy(idx, d, system);

      if (m_AOn == true)  // both A and B are active
        {
//         if (idx == 0 && d == 0)
//           std::cout << "Energy = " <<  ansA << " + " << m_RelativeEnergyScaling * ansB
//                     << " = " << ansA + m_RelativeEnergyScaling * ansB << std::endl;
        
        return (ansA + m_RelativeEnergyScaling * ansB + m_RelativeNormEnergyScaling * ansC);
        }
      else // only B is active
        {
        return ansB + m_RelativeNormEnergyScaling * ansC;
        }
      }
    else  // only A is active
      {
      return ansA;
      }

    return 0.0;;
  }
  
  virtual VectorType Evaluate(unsigned int idx, unsigned int d,
                              const ParticleSystemType *system,
                              double &maxmove, double &energy) const
  {
    double maxA, maxB, maxC;
    double energyA = 0.0;
    double energyB = 0.0;
    double energyC = 0.0;
    VectorType ansA;
    const_cast<ParticleDualVectorFunction *>(this)->m_Counter = m_Counter + 1.0;
    if (m_AOn == true)
      {
      ansA = m_FunctionA->Evaluate(idx, d, system, maxA, energyA);

      const_cast<ParticleDualVectorFunction *>(this)->m_AverageGradMagA = m_AverageGradMagA + ansA.magnitude();
      const_cast<ParticleDualVectorFunction *>(this)->m_AverageEnergyA = m_AverageEnergyA + energyA;
      }

    if (m_BOn == true)
      {
      VectorType ansB = m_FunctionB->Evaluate(idx, d, system, maxB, energyB);

      const_cast<ParticleDualVectorFunction *>(this)->m_AverageGradMagB = m_AverageGradMagB + ansB.magnitude();
      const_cast<ParticleDualVectorFunction *>(this)->m_AverageEnergyB = m_AverageEnergyB + energyB;

      VectorType ansC = m_FunctionC->Evaluate(idx, d, system, maxC, energyC);

      const_cast<ParticleDualVectorFunction *>(this)->m_AverageGradMagC = m_AverageGradMagC + ansC.magnitude();
      const_cast<ParticleDualVectorFunction *>(this)->m_AverageEnergyC = m_AverageEnergyC + energyC;

      if (m_AOn == true)  // both A and B are active
        {

//         if (idx == 0 && d == 0)
//           {
//           std::cout << "Gradient = " << ansA << " + " << m_RelativeGradientScaling * ansB << std::endl;
//           std::cout << "Energy = " << ansA << " + " << m_RelativeEnergyScaling * ansB << " = " <<
//             energyA + m_RelativeEnergyScaling * energyB << std::endl;
//           }

        if (maxA > maxB) { maxmove = maxB + m_RelativeNormEnergyScaling * maxC; }
        else { maxmove = maxA; }
        energy = energyA + m_RelativeEnergyScaling * energyB + m_RelativeNormEnergyScaling * energyC;
        return (ansA + m_RelativeGradientScaling * ansB + m_RelativeNormGradientScaling * ansC);
        }
      else // only B is active
        {
        maxmove = maxB + m_RelativeNormEnergyScaling * maxC ;
        energy = energyB + m_RelativeNormEnergyScaling * energyC;
        return (ansB + m_RelativeNormGradientScaling * ansC);
        }
      }
    else  // only A is active
      {
      maxmove = maxA;
      energy = energyA;
      return ansA;
      }

    // If nothing is turned on, then return a max time
    // step of 0 and a bogus vector.
    maxmove = 0.0;
    return ansA;
  }

  virtual void BeforeEvaluate(unsigned int idx, unsigned int d,
                              const ParticleSystemType *system)
  {
    if (m_AOn == true)
      {
      m_FunctionA->BeforeEvaluate(idx, d, system);
      }
    
    if (m_BOn == true)
      {
      m_FunctionB->BeforeEvaluate(idx, d, system);
      m_FunctionC->BeforeEvaluate(idx, d, system);
      }
  }

  /** This method is called by a solver after each iteration.  Subclasses may
      or may not implement this method.*/
  virtual void AfterIteration()
  {
    if (m_AOn) m_FunctionA->AfterIteration();
    if (m_BOn)
    {
      m_FunctionB->AfterIteration();
      m_FunctionC->AfterIteration();
    }
  }

  /** This method is called by a solver before each iteration.  Subclasses may
      or may not implement this method.*/
  virtual void BeforeIteration()
  {
    if (m_AOn) m_FunctionA->BeforeIteration();
    if (m_BOn)
    {
      m_FunctionB->BeforeIteration();
      m_FunctionC->BeforeIteration();
    }
    m_AverageGradMagA = 0.0;
    m_AverageGradMagB = 0.0;
    m_AverageGradMagC = 0.0;
    m_AverageEnergyA = 0.0;
    m_AverageEnergyB = 0.0;
    m_AverageEnergyC = 0.0;
    m_Counter = 0.0;
  }

  /** Some subclasses may require a pointer to the particle system and its
      domain number.  These methods set/get those values. */
  virtual void SetParticleSystem( ParticleSystemType *p)
  {
    Superclass::SetParticleSystem(p);
    if (m_FunctionA.GetPointer() != 0) m_FunctionA->SetParticleSystem(p);
    if (m_FunctionB.GetPointer() != 0) m_FunctionB->SetParticleSystem(p);
    if (m_FunctionC.GetPointer() != 0) m_FunctionC->SetParticleSystem(p);
  }

  void SetDomainNumber( unsigned int i)
  {
    Superclass::SetDomainNumber(i);
    if (m_FunctionA.GetPointer() != 0) m_FunctionA->SetDomainNumber(i);
    if (m_FunctionB.GetPointer() != 0) m_FunctionB->SetDomainNumber(i);
    if (m_FunctionC.GetPointer() != 0) m_FunctionC->SetDomainNumber(i);
  }

  void SetFunctionA( ParticleVectorFunction<VDimension> *o)
  {
    m_FunctionA = o;
    m_FunctionA->SetDomainNumber(this->GetDomainNumber());
    m_FunctionA->SetParticleSystem(this->GetParticleSystem());
  }

  void SetFunctionB( ParticleVectorFunction<VDimension> *o)
  {
    m_FunctionB = o;
    m_FunctionB->SetDomainNumber(this->GetDomainNumber());
    m_FunctionB->SetParticleSystem(this->GetParticleSystem());
  }

  void SetFunctionC( ParticleVectorFunction<VDimension> *o)
  {
    m_FunctionC = o;
    m_FunctionC->SetDomainNumber(this->GetDomainNumber());
    m_FunctionC->SetParticleSystem(this->GetParticleSystem());
  }


  /** Turn each term on and off. */
  void SetAOn()   { m_AOn = true;  }
  void SetAOff() { m_AOn = false;  }
  void SetAOn(bool s)  {    m_AOn = s;  }
  bool GetAOn() const {  return m_AOn;  }
  void SetBOn()   {  m_BOn = true; }
  void SetBOff() {  m_BOn = false;  }
  void SetBOn(bool s) {  m_BOn = s;  }
  bool GetBOn() const { return m_BOn;  }

  /** The relative scaling scales the gradient B relative to A.  By default
      this value is 1.0. */
  void SetRelativeEnergyScaling(double r)
  {
    m_RelativeEnergyScaling = r;
  }
  double GetRelativeEnergyScaling() const
  {
    return m_RelativeEnergyScaling;
  }
  void SetRelativeNormEnergyScaling(double r)
  {
    m_RelativeNormEnergyScaling = r;
  }
  double GetRelativeNormEnergyScaling() const
  {
    return m_RelativeNormEnergyScaling;
  }

  void SetRelativeGradientScaling(double r)
  {
    m_RelativeGradientScaling = r;
  }
  double GetRelativeGradientScaling() const
  {
    return m_RelativeGradientScaling;
  }
  void SetRelativeNormGradientScaling(double r)
  {
    m_RelativeNormGradientScaling = r;
  }
  double GetRelativeNormGradientScaling() const
  {
    return m_RelativeNormGradientScaling;
  }
  
  double GetAverageGradMagA() const
  {
    if (m_Counter != 0.0) return m_AverageGradMagA / m_Counter;
    else return 0.0;
  }
  double GetAverageGradMagB() const
  {
    if (m_Counter != 0.0) return m_AverageGradMagB / m_Counter;
    else return 0.0;
  }
  double GetAverageGradMagC() const
  {
    if (m_Counter != 0.0) return m_AverageGradMagC / m_Counter;
    else return 0.0;
  }

  double GetAverageEnergyA() const
  {
    if (m_Counter != 0.0) return m_AverageEnergyA / m_Counter;
    else return 0.0;
  }
  double GetAverageEnergyB() const
  {
    if (m_Counter != 0.0) return m_AverageEnergyB / m_Counter;
    else return 0.0;
  }
  double GetAverageEnergyC() const
  {
    if (m_Counter != 0.0) return m_AverageEnergyC / m_Counter;
    else return 0.0;
  }
  
protected:
  ParticleDualVectorFunction() : m_AOn(true), m_BOn(false),
                                 m_RelativeGradientScaling(1.0),
                                 m_RelativeEnergyScaling(1.0),
                                 m_RelativeNormGradientScaling(0.0),
                                 m_RelativeNormEnergyScaling(0.0)  {}

  virtual ~ParticleDualVectorFunction() {}
  void operator=(const ParticleDualVectorFunction &);
  ParticleDualVectorFunction(const ParticleDualVectorFunction &);

  bool m_AOn;
  bool m_BOn;
  double m_RelativeGradientScaling;
  double m_RelativeEnergyScaling;
  double m_RelativeNormGradientScaling;
  double m_RelativeNormEnergyScaling;
  double m_AverageGradMagA;
  double m_AverageGradMagB;
  double m_AverageGradMagC;
  double m_AverageEnergyA;
  double m_AverageEnergyB;
  double m_AverageEnergyC;
  double m_Counter;
  
  typename ParticleVectorFunction<VDimension>::Pointer m_FunctionA;
  typename ParticleVectorFunction<VDimension>::Pointer m_FunctionB;
  typename ParticleVectorFunction<VDimension>::Pointer m_FunctionC;
};


} //end namespace

#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleDualVectorFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "itkParticleDualVectorFunction.txx"
#endif

#endif
