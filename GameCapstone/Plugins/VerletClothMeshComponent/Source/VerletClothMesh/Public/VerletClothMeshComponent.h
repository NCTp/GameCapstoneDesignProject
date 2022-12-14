// VerletClothComponent.h - VerletClothMeshComponent Plugin - Niall Horn 2020. 

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"

#include "VerletClothMeshComponent.generated.h"

#define INLINE __forceinline

class  HashGrid; 
struct FVerletClothConstraint;

// --- VerletClothParticle - Basic Struct for storing Particle Representation of cloth. ---
struct FVerletClothParticle
{
	FVerletClothParticle()
		: Position(0, 0, 0)
		, PrevPosition(0, 0, 0)
		, Force(0, 0, 0)
		, Col(255, 255, 255, 255)
		, ID(-1)
		, C_idx(-1)
		, state(1)
		, conCount(0)
	{}
	FVector Position;
	FVector PrevPosition;
	FVector Force;
	FColor Col;

	int32 ID;
	uint32 C_idx; 
	int8 state, conCount;
};


UCLASS(hidecategories = (Object, Physics, Activation, Collision, Navigation, Transform, "Components|Activation"), editinlinenew, meta = (BlueprintSpawnableComponent), ClassGroup = Rendering)
class VERLETCLOTHMESH_API UVerletClothMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UVerletClothMeshComponent(const FObjectInitializer& ObjectInitializer);

	// --- UActorComponent Overrides ---
	void OnRegister() override; 
	void TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	// Properties

	// --- Verlet Cloth - Properties - Cloth Setup ---
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Static Mesh")
	UStaticMeshComponent *sm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation")
	bool bShowStaticMesh;

	// --- VerletCloth - Properties - Cloth Simulation ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation")
		bool bSimulate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cloth Simulation", meta = (ClampMin = "0.005", UIMin = "0.005", UIMax = "0.1"))
		float SubstepTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation", meta = (ClampMin = "1", ClampMax = "16"))
		int32 ConstraintIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation")
		bool bWorldCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation")
		bool bSelfCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cloth Simulation", meta = (ClampMin = "0.01", ClampMax = "1000.0"))
		float ParticleMass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cloth Simulation", meta = (ClampMin = "0.01", ClampMax = "1000.0"))
		float ParticleRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cloth Simulation", meta = (ClampMin = "0.05", ClampMax = "0.99"))
		float StiffnessCoefficent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation", meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "bWorldCollision"))
		float CollisionFriction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation")
		FVector ClothForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation")
		bool bUse_VolumePressureForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation", meta = (ClampMin = "2", ClampMax = "1000", EditCondition = "bUse_VolumePressureForce"))
		int32 VolSample_Count;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation", meta = (EditCondition = "bUse_VolumePressureForce"))
		float VolPressure_Coefficient;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation")
		float ClothGravityScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation")
		FVector WindDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation")
		float WindScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation")
		bool bUse_Sleeping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation", meta = (EditCondition = "bUse_Sleeping"))
		float Sleep_DeltaThreshold;

	// --- VerletCloth - Properties - Self Collision ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation", meta = (ClampMin = "1", ClampMax = "8"))
		int32 SelfColIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation", meta = (ClampMin = "0", ClampMax = "100"))
		int32 Cells_PerDim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cloth Simulation", meta = (UIMin = "1.0", UIMax = "10000.0"))
		float Grid_Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation")
		bool bShow_Grid;

	// --- VerletCloth - Properties - Cloth Simulation Debug ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation Debug", meta = (EditCondition = "bUse_Sleeping"))
		bool bShow_Sleeping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Simulation Debug")
		bool bShow_Constraints;

	// --- Component UFunctions - Cloth Setup ---
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Cloth Simulation")
	void BuildClothState();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Cloth Simulation")
	void ResetToInitalState();

	// --- Component UFunctions - Debug Editor Draw ---
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Cloth Simulation Debug")
	void DBG_ShowParticles() const;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Cloth Simulation Debug")
	void DBG_ShowTangents();

	//UFUNCTION(BlueprintCallable, CallInEditor, Category = "Cloth Simulation Debug")
	//void DBG_ShowAdjacency() const;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Cloth Simulation Debug", meta = (EditCondition = "bSelfCollision"))
	void DBG_ShowHash();

private:
	// --- Cloth Solver Methods ---
	void BuildTriArrays();

	void BuildClothConstraints();

	void EvalClothConstraints();

	void TickUpdateCloth();

	void UpdateTangents(TArray<FProcMeshTangent> &out_Tangents, TArray<FVector> &out_Normals); 

	void ClothCollisionWorld();

	void ClothCollisionSelf(HashGrid *hg);

	void Integrate(float InSubstepTime);

	static void SolveDistanceConstraint(FVerletClothParticle &ParticleA, FVerletClothParticle &ParticleB, float RestDistance, float StiffnessCoeff);

	void SubstepSolve();

	//float ClothWorldCollision_AvgPosDelta();
	//bool GetSleepState();

	void ShowVelCol(); 

	// Volume Methods

	void GetVolSamplePts(int32 n); 

	float CalcClothVolume(); // Using Sample Pts. 

	void VolumePressureForce(int32 mode = 0);

	void VolumePreservation(); // Per Substep. 

	INLINE float SquareDist(const FVector &A, const FVector &B);

	// --- Cloth Data ---
	TArray<FVerletClothParticle> Particles;
	TArray<FVerletClothConstraint> Constraints; 
	TArray<FVector> Normals; 
	TArray<FVerletClothParticle*> VolSamplePts;
	float restVolume, curVolume, deltaVolume; 
	int32 particleCount;

	// --- State Flags --- 
	bool clothStateExists, world_collided;


	// --- Simulation Settings ---
	float Dt, At, St; // Delta, Accumulated, Substep Time
	float pr2sqr;     // 2ParticleRadius^2

	// --- Static Mesh Data ---
	struct
	{
		// SM Deserialized
		TArray<FVector>            Pos;
		TArray<FColor>             Col;
		TArray<FVector>            Normal;
		TArray<FProcMeshTangent>   Tang;
		TArray<FVector2D>          UV;
		TArray<int32>              Ind;
		TArray<FIntVector>         Tris; 

		// Vert Shared Tris
		TArray<int32> *vtris;

		// SM Buffer Ptrs
		FPositionVertexBuffer   *vb;
		FStaticMeshVertexBuffer *smvb;
		FColorVertexBuffer      *cvb;
		FRawStaticIndexBuffer   *ib;

		int32 vert_count, ind_count, adj_count, tri_count; 
		bool has_uv, has_col; 
	} smData;

	friend struct FVerletClothConstraint;
	friend class  HashGrid;
};

// --- Inline Memember Function Implementations --- 

// Square Distance Between 2 Position Vectors. 
float UVerletClothMeshComponent::SquareDist(const FVector &A, const FVector &B)
{
	return FMath::Square((B.X - A.X)) + FMath::Square((B.Y - A.Y)) + FMath::Square((B.Z - A.Z));
}

// --- VerletClothConstraint Struct --- 

// Basic Structure for storing Cloth Constraints to evaluate. 
struct FVerletClothConstraint
{
	FVerletClothConstraint(FVerletClothParticle &Pt_0, FVerletClothParticle &Pt_1, UVerletClothMeshComponent *cloth)
		: Pt0(Pt_0), Pt1(Pt_1), Cloth(cloth)
	{
		// Get Particles Corresponding Vertices Orginal Postions and Rest Length.
		orgP0 = Cloth->smData.Pos[Pt_0.ID]; orgP1 = Cloth->smData.Pos[Pt_1.ID];
		restLength = (orgP1 - orgP0).Size();
		// ID to Identify Particle/Vertex ID Pair of Constraint. 
		conID = Pt_0.ID * Pt_1.ID;
	}
	FVerletClothConstraint() = delete;

	FVerletClothParticle &Pt0, &Pt1;
	FVector orgP0, orgP1;
	float restLength;
	int32 conID;
	UVerletClothMeshComponent *Cloth;
};
