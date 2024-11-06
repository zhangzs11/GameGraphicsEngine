//#pragma once
//
//// Includes
////=========
//
//#include "Configuration.h"
//
//#include <cstdint>
//#include <vector>
//#include <Engine/Results/Results.h>
//#include <Engine/Assets/ReferenceCountedAssets.h>
//
//// Class Declaration
////==================
//
//namespace eae6320
//{
//    namespace Graphics
//    {
//        class cEffect
//        {
//        public:
//            // This defines the actual reference counting functions
//            // This must be public
//            EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()
//
//            // Bind the effect
//            virtual void Bind() const = 0;
//
//
//            // factory function
//            // In the child
//            // static cResult CreateEffect();
//
//        private:
//            // This prevents the class or struct from using illegal functions
//            EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cEffect)
//
//            // This declares the reference count member variable
//            EAE6320_ASSETS_DECLAREREFERENCECOUNT()
//
//            cEffect() = default;
//            // ~cEffect();
//
//            // Initialize the effect with shader paths
//            // In the child
//            // cResult Initialize()
//            
//            // Clean up the effect
//            virtual cResult CleanUp() = 0;
//        };
//    }
//}
//
