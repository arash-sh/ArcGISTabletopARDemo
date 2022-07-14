#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISSpatialReference.h"
#include "ArcGISMapsSDK/Components/ArcGISLocationComponent.h"

#define TestFlags (EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FArcGISLocationComponentPositionTest, "ArcGISMapsSDK.ArcGISLocationComponent.Position", TestFlags)
bool FArcGISLocationComponentPositionTest::RunTest(const FString& Parameters)
{
	double Tolerance = 0.001;
	auto Location = NewObject<UArcGISLocationComponent>();
	auto SR = UArcGISSpatialReference::CreateArcGISSpatialReference(4326);
	auto InPosition = UArcGISPoint::CreateArcGISPointWithXYZSpatialReference(10, 20.5, 30, SR);
	Location->SetPosition(InPosition);
	auto OutPosition = Location->GetPosition();

	TestEqual("X value mismatched", OutPosition->GetX(), InPosition->GetX(), Tolerance);
	TestEqual("Y value mismatched", OutPosition->GetY(), InPosition->GetY(), Tolerance);
	TestEqual("Z value mismatched", OutPosition->GetZ(), InPosition->GetZ(), Tolerance);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FArcGISLocationComponentRotationTest, "ArcGISMapsSDK.ArcGISLocationComponent.Rotation", TestFlags)
bool FArcGISLocationComponentRotationTest::RunTest(const FString& Parameters)
{
	double Tolerance = 0.001;
	auto Location = NewObject<UArcGISLocationComponent>();
	auto InRotation = UArcGISRotation::CreateArcGISRotation(10, 20.5, 30);
	Location->SetRotation(InRotation);
	auto OutRotation = Location->GetRotation();

	TestEqual("Heading value mismatched", OutRotation->GetHeading(), InRotation->GetHeading(), Tolerance);
	TestEqual("Pitch value mismatched", OutRotation->GetPitch(), InRotation->GetPitch(), Tolerance);
	TestEqual("Roll value mismatched", OutRotation->GetRoll(), InRotation->GetRoll(), Tolerance);

	return true;
}
