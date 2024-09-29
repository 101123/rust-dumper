#include <cstdint>

namespace GameAssembly {
	constexpr const static size_t timestamp = 0x66f64428;
	constexpr const static size_t gc_handles = 0xce23b30;
	constexpr const static size_t ConVar_Server_TypeInfo = 0xcbb3e70;
}

namespace BaseNetworkable {
	
// Offsets
	constexpr const static size_t prefabID = 0x30;
	constexpr const static size_t net = 0x28;
	constexpr const static size_t parentEntity = 0x60;
	constexpr const static size_t children = 0x50;
}

// obf name: ::%a5a57d356d20eccdc4bf9843f60fbbb659a64139
#define BaseNetworkable_Static_ClassName "BaseNetworkable/%a5a57d356d20eccdc4bf9843f60fbbb659a64139"

namespace BaseNetworkable_Static {
	
// Offsets
	constexpr const static size_t clientEntities = 0x10;
}

// obf name: ::%f6ed05ce032d6576814b9cc9c4c7b670da391188
#define BaseNetworkable_EntityRealm_ClassName "BaseNetworkable/%f6ed05ce032d6576814b9cc9c4c7b670da391188"

namespace BaseNetworkable_EntityRealm {
	
// Offsets
	constexpr const static size_t entityList = 0x10;
	
// Functions
	constexpr const static size_t Find = 0x3417d80;
}

// obf name: ::%330fd55f6437c9b78cd206884244854ebd83151f
#define System_ListDictionary_ClassName "%330fd55f6437c9b78cd206884244854ebd83151f<%ef97f3fa803edd000f2f8d8a708a99a63e4a2977,BaseNetworkable>"

namespace System_ListDictionary {
	
// Offsets
	constexpr const static size_t vals = 0x10;
}

namespace Model {
	
// Offsets
	constexpr const static size_t boneTransforms = 0x50;
}

namespace BaseEntity {
	
// Offsets
	constexpr const static size_t bounds = 0xa0;
	constexpr const static size_t model = 0xc8;
	constexpr const static size_t flags = 0xd0;
	constexpr const static size_t positionLerp = 0x98;
	
// Functions
	constexpr const static size_t FindBone = 0x4073c10;
	constexpr const static size_t GetWorldVelocity = 0x40b7300;
	constexpr const static size_t GetParentVelocity = 0x408fb70;
}

// obf name: ::%de5763a6c28620d2050a4e94f0d2e6d4af224c14
#define PositionLerp_ClassName "%de5763a6c28620d2050a4e94f0d2e6d4af224c14"

namespace PositionLerp {
	
// Offsets
	constexpr const static size_t interpolator = 0x48;
}

// obf name: ::%1ff578110b76f87710fc820b143f60924dcbf368
#define Interpolator_ClassName "%1ff578110b76f87710fc820b143f60924dcbf368<%1f3010a01b0ae51befcbea13bba742762ec39321>"

namespace Interpolator {
	constexpr const static size_t List = 0x30;
}

namespace BaseCombatEntity {
	
// Offsets
	constexpr const static size_t lifestate = 0x214;
	constexpr const static size_t markAttackerHostile = 0x21a;
	constexpr const static size_t _health = 0x21c;
	constexpr const static size_t _maxHealth = 0x220;
}

namespace ItemDefinition {
	
// Offsets
	constexpr const static size_t itemid = 0x20;
	constexpr const static size_t category = 0x48;
	constexpr const static size_t shortname = 0x28;
	constexpr const static size_t displayName = 0x30;
}

namespace RecoilProperties {
	
// Offsets
	constexpr const static size_t recoilYawMin = 0x18;
	constexpr const static size_t recoilYawMax = 0x1c;
	constexpr const static size_t recoilPitchMin = 0x20;
	constexpr const static size_t recoilPitchMax = 0x24;
	constexpr const static size_t overrideAimconeWithCurve = 0x5c;
	constexpr const static size_t aimconeProbabilityCurve = 0x70;
	constexpr const static size_t newRecoilOverride = 0x80;
}

namespace BaseProjectile_Magazine {
	
// Offsets
	constexpr const static size_t capacity = 0x18;
	constexpr const static size_t contents = 0x1c;
	constexpr const static size_t ammoType = 0x20;
}

namespace AttackEntity {
	
// Offsets
	constexpr const static size_t deployDelay = 0x220;
	constexpr const static size_t repeatDelay = 0x224;
	constexpr const static size_t animationDelay = 0x228;
	constexpr const static size_t noHeadshots = 0x276;
	constexpr const static size_t nextAttackTime = 0x278;
	constexpr const static size_t timeSinceDeploy = 0x290;
}

namespace BaseProjectile {
	
// Offsets
	constexpr const static size_t projectileVelocityScale = 0x2c4;
	constexpr const static size_t automatic = 0x2c8;
	constexpr const static size_t primaryMagazine = 0x300;
	constexpr const static size_t aimSway = 0x31c;
	constexpr const static size_t aimSwaySpeed = 0x320;
	constexpr const static size_t recoil = 0x328;
	constexpr const static size_t aimconeCurve = 0x330;
	constexpr const static size_t aimCone = 0x338;
	constexpr const static size_t cachedModHash = 0x38c;
	constexpr const static size_t sightAimConeScale = 0x390;
	constexpr const static size_t sightAimConeOffset = 0x394;
	constexpr const static size_t hipAimConeScale = 0x398;
	constexpr const static size_t hipAimConeOffset = 0x39c;
	
// Functions
	constexpr const static size_t LaunchProjectileClientSide = 0x5464770;
	constexpr const static size_t ScaleRepeatDelay = 0x54565a0;
	constexpr const static size_t GetAimCone = 0x5447710;
	constexpr const static size_t UpdateAmmoDisplay = 0x5443e80;
}

// obf name: ::%44b9ccb5e24b45da88d19c1a57d399bd7b2bbc1b
#define BaseProjectile_Static_ClassName "BaseProjectile/%44b9ccb5e24b45da88d19c1a57d399bd7b2bbc1b"

namespace BaseProjectile_Static {
	
// Offsets
	constexpr const static size_t createdProjectiles = 0x38;
}

// obf name: ::%913af6a9ac76a4e6c2b0743ef9ab39770804c9c1
#define HitTest_ClassName "%913af6a9ac76a4e6c2b0743ef9ab39770804c9c1"

namespace HitTest {
	
// Offsets
	constexpr const static size_t type = 0x7c;
	constexpr const static size_t AttackRay = 0x64;
	constexpr const static size_t RayHit = 0x38;
	constexpr const static size_t damageProperties = 0x90;
	constexpr const static size_t gameObject = 0x28;
	constexpr const static size_t collider = 0x98;
	constexpr const static size_t ignoredTypes = 0x88;
	constexpr const static size_t HitTransform = 0xd0;
	constexpr const static size_t HitPart = 0x30;
	constexpr const static size_t HitMaterial = 0xc0;
	constexpr const static size_t DidHit = 0xd8;
	constexpr const static size_t MaxDistance = 0x80;
	constexpr const static size_t HitPoint = 0x18;
	constexpr const static size_t HitPoint = 0xa0;
	constexpr const static size_t ignoreEntity = 0x10;
	constexpr const static size_t HitEntity = 0xb0;
}

namespace BaseMelee {
	
// Offsets
	constexpr const static size_t damageProperties = 0x2b8;
	constexpr const static size_t maxDistance = 0x2d0;
	constexpr const static size_t gathering = 0x308;
	
// Functions
	constexpr const static size_t ProcessAttack = 0x4db1500;
	constexpr const static size_t DoThrow = 0x4db5010;
}

namespace FlintStrikeWeapon {
	
// Offsets
	constexpr const static size_t strikeRecoil = 0x3d8;
	constexpr const static size_t _didSparkThisFrame = 0x3e0;
}

namespace CompoundBowWeapon {
	
// Offsets
	constexpr const static size_t stringHoldDurationMax = 0x3e8;
}

// obf name: ::%4791f60d554885b9157537eb5fd9cb8102563e9e
#define ItemContainer_ClassName "%4791f60d554885b9157537eb5fd9cb8102563e9e"

namespace ItemContainer {
	
// Offsets
	constexpr const static size_t uid = 0x48;
	constexpr const static size_t itemList = 0x58;
	
// Functions
	constexpr const static size_t FindItemsByItemID = 0x46e9a90;
}

namespace PlayerInventory {
	
// Offsets
	constexpr const static size_t containerBelt = 0x28;
	constexpr const static size_t containerWear = 0x58;
	constexpr const static size_t containerMain = 0x60;
	
// Functions
	constexpr const static size_t Initialize = 0x1868360;
	constexpr const static size_t FindItemsByItemID = 0x186a150;
}

// obf name: ::%409c43cce848c77686735227356ae23726d7d4f3
#define InputMessage_ClassName "%409c43cce848c77686735227356ae23726d7d4f3"

namespace InputMessage {
	
// Offsets
	constexpr const static size_t buttons = 0x20;
}

// obf name: ::%5fa5094f6fee6553bc596d74d07039e57fb5cdfc
#define InputState_ClassName "%5fa5094f6fee6553bc596d74d07039e57fb5cdfc"

namespace InputState {
	
// Offsets
	constexpr const static size_t current = 0x20;
	constexpr const static size_t previous = 0x18;
}

namespace PlayerInput {
	
// Offsets
	constexpr const static size_t state = 0x28;
	constexpr const static size_t bodyAngles = 0x44;
}

namespace PlayerEyes {
	
// Offsets
	constexpr const static size_t viewOffset = 0x40;
	constexpr const static size_t bodyRotation = 0x0;
	
// Functions
	constexpr const static size_t get_position = 0x1e0e900;
	constexpr const static size_t get_rotation = 0x1e04030;
	constexpr const static size_t HeadForward = 0x1e10090;
}

// obf name: ::%c733ff246531e3236e8eb80da665c399f2230f53
#define PlayerBelt_ClassName "%c733ff246531e3236e8eb80da665c399f2230f53"

namespace PlayerBelt {
	
// Functions
	constexpr const static size_t ChangeSelect = 0x42bd990;
	constexpr const static size_t GetActiveItem = 0x42cc700;
}

namespace BasePlayer {
	
// Offsets
	constexpr const static size_t playerModel = 0x3e0;
	constexpr const static size_t input = 0x3c0;
	constexpr const static size_t movement = 0x280;
	constexpr const static size_t currentTeam = 0x3f0;
	constexpr const static size_t clActiveItem = 0x430;
	constexpr const static size_t modelState = 0x2e8;
	constexpr const static size_t playerFlags = 0x530;
	constexpr const static size_t eyes = 0x2d8;
	constexpr const static size_t playerRigidbody = 0x240;
	constexpr const static size_t userID = 0x578;
	constexpr const static size_t inventory = 0x3d8;
	constexpr const static size_t _displayName = 0x2a0;
	constexpr const static size_t _lookingAt = 0x250;
	constexpr const static size_t lastSentTickTime = 0x4ec;
	constexpr const static size_t lastSentTick = 0x560;
	constexpr const static size_t mounted = 0x470;
	constexpr const static size_t Belt = 0x2d0;
	
// Functions
	constexpr const static size_t ChatMessage = 0x105e440;
	constexpr const static size_t ClientInput = 0x6d40c0;
	constexpr const static size_t IsOnGround = 0x1014f90;
	constexpr const static size_t GetHeldItemID = 0x100deb0;
	constexpr const static size_t GetSpeed = 0x1018e20;
	constexpr const static size_t CanBuild = 0x10031d0;
}

namespace BaseMovement {
	
// Functions
	constexpr const static size_t set_TargetMovement = 0x157fd20;
}

namespace BuildingPrivlidge {
	
// Offsets
	constexpr const static size_t allowedConstructionItems = 0x330;
	constexpr const static size_t cachedProtectedMinutes = 0x338;
}

namespace WorldItem {
	
// Offsets
	constexpr const static size_t allowPickup = 0x190;
	constexpr const static size_t item = 0x198;
}

namespace HackableLockedCrate {
	
// Offsets
	constexpr const static size_t timerText = 0x348;
}

namespace ProjectileWeaponMod {
	
// Offsets
	constexpr const static size_t projectileVelocity = 0x1b0;
}

namespace PlayerWalkMovement {
	
// Offsets
	constexpr const static size_t capsule = 0xc0;
	constexpr const static size_t ladder = 0xa8;
	constexpr const static size_t groundTime = 0xe0;
	constexpr const static size_t modify = 0x14c;
}

// obf name: ::%b04a3fc8d32f84b6a07b599e9dafdb9712abdf02
#define ConsoleSystem_ClassName "%b04a3fc8d32f84b6a07b599e9dafdb9712abdf02"

namespace ConsoleSystem {
	
// Functions
	constexpr const static size_t Run = 0x6a68fe0;
}

// obf name: ::%db00e28fe5a2036c32cdce4aef18933da3ea5ea0
#define ConsoleSystem_Option_ClassName "%b04a3fc8d32f84b6a07b599e9dafdb9712abdf02/%db00e28fe5a2036c32cdce4aef18933da3ea5ea0"

namespace ConsoleSystem_Option {
	
// Offsets
}

// obf name: ::%96912177d8b930cdefe55f3deffbdf73ac645003
#define ConsoleSystem_Command_ClassName "%b04a3fc8d32f84b6a07b599e9dafdb9712abdf02/%96912177d8b930cdefe55f3deffbdf73ac645003"

namespace ConsoleSystem_Command {
	
// Offsets
	constexpr const static size_t AllowRunFromServer = 0x0;
}

namespace LootableCorpse {
	
// Offsets
	constexpr const static size_t playerSteamID = 0x278;
	constexpr const static size_t _playerName = 0x268;
}

namespace DroppedItemContainer {
	
// Offsets
	constexpr const static size_t playerSteamID = 0x248;
	constexpr const static size_t _playerName = 0x230;
}

namespace MainCamera {
	
// Offsets 
	constexpr const static size_t mainCamera = 0x20;
	constexpr const static size_t mainCameraTransform = 0x28;
	
// Functions
	constexpr const static size_t Update = 0x1a56bf0;
	constexpr const static size_t Trace = 0x1a52b60;
}

// obf name: ::%431a5890731c798fd31059e84a188af72afdc27a
#define LocalPlayer_ClassName "%431a5890731c798fd31059e84a188af72afdc27a"

namespace LocalPlayer {
	
// Functions
	constexpr const static size_t ItemCommand = 0x45239f0;
	constexpr const static size_t MoveItem = 0x4541760;
	constexpr const static size_t get_Entity = 0x4527aa0;
}

// obf name: ::%07a26f91ba3acf90d02b01610ab21da26af1eb29
#define PlayerTick_ClassName "%07a26f91ba3acf90d02b01610ab21da26af1eb29"

namespace PlayerTick {
	
// Offsets
	constexpr const static size_t inputState = 0x30;
	constexpr const static size_t modelState = 0x48;
	constexpr const static size_t activeItem = 0x40;
	constexpr const static size_t parentID = 0x20;
	constexpr const static size_t position = 0x54;
	constexpr const static size_t eyePos = 0x10;
}

// obf name: ::%a02619e400e004b742e1f89ef2c6f4bfd8d26458
#define ModelState_ClassName "%a02619e400e004b742e1f89ef2c6f4bfd8d26458"

namespace ModelState {
	
// Offsets
	constexpr const static size_t waterLevel = 0x30;
	constexpr const static size_t flags = 0x4c;
}

// obf name: ::%b7438f3a83aea4f54a11feef5b947427bdf425d9
#define Item_ClassName "%b7438f3a83aea4f54a11feef5b947427bdf425d9"

namespace Item {
	
// Offsets
	constexpr const static size_t info = 0x58;
	constexpr const static size_t uid = 0xa0;
	constexpr const static size_t _maxCondition = 0x54;
	constexpr const static size_t _condition = 0x60;
	constexpr const static size_t position = 0x8c;
	constexpr const static size_t amount = 0xb4;
	constexpr const static size_t contents = 0x70;
	constexpr const static size_t parent = 0x98;
	constexpr const static size_t worldEnt = 0x28;
	constexpr const static size_t heldEntity = 0x78;
}

// obf name: ::%708f8b61f5b81f580e1c0632388c2a6c2cb0fa85
#define WaterLevel_ClassName "%708f8b61f5b81f580e1c0632388c2a6c2cb0fa85"

namespace WaterLevel {
	
// Functions
	constexpr const static size_t Test = 0x2896be0;
	constexpr const static size_t GetWaterLevel = 0x289afa0;
}

// obf name: ::%742fb30faaca3f31e92bceb3e5f4034f63bda7d9
#define ConVar_Graphics_ClassName "%742fb30faaca3f31e92bceb3e5f4034f63bda7d9"

namespace ConVar_Graphics {
	
// Functions
	constexpr const static size_t _fov_getter = 0x1a86e80;
	constexpr const static size_t _fov_setter = 0x1a97f80;
}

// obf name: ::%bd4547fa34247746883c7466aa8a96a3a6fca78b
#define ConVar_Graphics_Static_ClassName "%742fb30faaca3f31e92bceb3e5f4034f63bda7d9/%bd4547fa34247746883c7466aa8a96a3a6fca78b"

namespace ConVar_Graphics_Static {
	
// Offsets
	constexpr const static size_t _fov = 0x5a4;
}

namespace BaseFishingRod {
	
// Offsets
	constexpr const static size_t CurrentState = 0x240;
	constexpr const static size_t currentBobber = 0x248;
	constexpr const static size_t clientStrainAmountNormalised = 0x2b0;
	
// Functions
	constexpr const static size_t UpdateLineRenderer = 0x3a94ce0;
	constexpr const static size_t EvaluateFishingPosition = 0x3aa1a40;
}

namespace FishingBobber {
	
// Offsets
	constexpr const static size_t bobberRoot = 0x248;
}

namespace GameManifest {
	
// Functions
	constexpr const static size_t GUIDToObject = 0x4ca81b0;
}

// obf name: ::%e58a9317e0e60054d89436620513ba0bacc4081f
#define GameManager_ClassName "%e58a9317e0e60054d89436620513ba0bacc4081f"

namespace GameManager {
	
// Functions
	constexpr const static size_t CreatePrefab = 0x3c3bba0;
}

// obf name: ::%404f1b22d294961ebcf889769449a2b8d2a5f4df
#define GameManager_Static_ClassName "%e58a9317e0e60054d89436620513ba0bacc4081f/%404f1b22d294961ebcf889769449a2b8d2a5f4df"

namespace GameManager_Static {
	
// Offsets
	constexpr const static size_t client = 0x20;
}

namespace ItemModProjectile {
	
// Offsets
	constexpr const static size_t projectileObject = 0x20;
	constexpr const static size_t ammoType = 0x30;
	constexpr const static size_t projectileSpread = 0x38;
	constexpr const static size_t projectileVelocity = 0x3c;
	constexpr const static size_t projectileVelocitySpread = 0x40;
	constexpr const static size_t useCurve = 0x44;
	constexpr const static size_t spreadScalar = 0x48;
}

namespace Projectile {
	
// Offsets
	constexpr const static size_t initialVelocity = 0x20;
	constexpr const static size_t drag = 0x2c;
	constexpr const static size_t gravityModifier = 0x30;
	constexpr const static size_t thickness = 0x34;
	constexpr const static size_t initialDistance = 0x38;
	constexpr const static size_t swimScale = 0xbc;
	constexpr const static size_t swimSpeed = 0xc8;
	constexpr const static size_t owner = 0x1a0;
	constexpr const static size_t sourceProjectilePrefab = 0xd8;
	
// Functions
	constexpr const static size_t DoHit = 0x5a324d0;
}

namespace ProjectileShoot {
	
// Offsets
	constexpr const static size_t projectiles = 0x0;
}

namespace ProjectileShoot_Projectile {
	
// Offsets
	constexpr const static size_t projectileID = 0x0;
	constexpr const static size_t startPos = 0x0;
	constexpr const static size_t startVel = 0x0;
	constexpr const static size_t seed = 0x0;
}

namespace CraftingQueue {
	
// Offsets
	constexpr const static size_t icons = 0x30;
}

// obf name: ::%b94e944307d5a44b88d968cf1f26a6ca43ffa939
#define CraftingQueue_Static_ClassName "CraftingQueue/%b94e944307d5a44b88d968cf1f26a6ca43ffa939"

namespace CraftingQueue_Static {
	
// Offsets
	constexpr const static size_t isCrafting = 0x10;
}

namespace CraftingQueueIcon {
	
// Offsets
	constexpr const static size_t endTime = 0x4c;
	constexpr const static size_t item = 0x60;
}

namespace Planner {
	
// Offsets
	constexpr const static size_t currentConstruction = 0x268;
}

namespace Construction {
	
// Offsets
	constexpr const static size_t holdToPlaceDuration = 0xf4;
}

// obf name: ::HeldEntity
#define HeldEntity_ClassName "HeldEntity"

namespace HeldEntity {
	
// Offsets
	constexpr const static size_t _punches = 0x1b0;
	constexpr const static size_t viewModel = 0x208;
	
// Functions
	constexpr const static size_t OnDeploy = 0x4365000;
}

// obf name: ::%762be1f46acba910195d3aa3c9e36879c98391a3
#define PunchEntry_ClassName "HeldEntity/%762be1f46acba910195d3aa3c9e36879c98391a3"

namespace PunchEntry {
	
// Offsets
	constexpr const static size_t startTime = 0x10;
	constexpr const static size_t duration = 0x2c;
	constexpr const static size_t amount = 0x14;
	constexpr const static size_t amountAdded = 0x20;
}

namespace ViewModel {
	
// Offsets
	constexpr const static size_t instance = 0x28;
}

namespace MedicalTool {
	
// Offsets
	constexpr const static size_t resetTime = 0x2d8;
}

namespace WaterSystem {
	
// Functions
	constexpr const static size_t get_Ocean = 0x5254300;
}

namespace WaterBody {
	
// Offsets
	constexpr const static size_t meshFilter = 0x58;
}

namespace TerrainMeta {
	
// Offsets
	constexpr const static size_t Terrain = 0x118;
	constexpr const static size_t Position = 0x18;
	constexpr const static size_t Size = 0x24;
	constexpr const static size_t OneOverSize = 0x30;
	constexpr const static size_t Collision = 0x668;
	constexpr const static size_t HeightMap = 0x108;
	constexpr const static size_t SplatMap = 0x598;
}

namespace TerrainCollision {
	
// Functions
	constexpr const static size_t GetIgnore = 0xf5a40000;
}

namespace TerrainHeightMap {
	
// Offsets
	constexpr const static size_t normY = 0x60;
}

namespace TerrainSplatMap {
	
// Offsets
	constexpr const static size_t num = 0x60;
}

// obf name: ::%5ce5a76f3c0b5cd6e6714d4f8ae73b99ea362aa0
#define World_Static_ClassName "%20106acf77ce03933479f3e39091e211e1476862/%5ce5a76f3c0b5cd6e6714d4f8ae73b99ea362aa0"

namespace World_Static {
	
// Offsets
	constexpr const static size_t _size = 0xf4;
}

namespace ItemIcon {
	
// Functions
	constexpr const static size_t SetTimedLootAction = 0x1c45b10;
}

// obf name: ::%2963ae9bc12844fa81d790b7df7be9588325236b
#define ItemIcon_Static_ClassName "ItemIcon/%2963ae9bc12844fa81d790b7df7be9588325236b"

namespace ItemIcon_Static {
	
// Offsets
	constexpr const static size_t containerLootStartTimes = 0x40;
}

// obf name: ::%b67549f89a147cc083e059a5ff8085678bff072b
#define Effect_ClassName "%b67549f89a147cc083e059a5ff8085678bff072b"

namespace Effect {
	
// Offsets
	constexpr const static size_t pooledString = 0xa0;
	constexpr const static size_t worldPos = 0x90;
}

// obf name: ::%61005473c3de98b766e11a2567cc784ea6b66b90
#define EffectNetwork_ClassName "%61005473c3de98b766e11a2567cc784ea6b66b90"

namespace EffectNetwork {
}

// obf name: ::%4971d643abdeeeffc046a2d066d632a05b6d860a
#define EffectNetwork_Static_ClassName "%61005473c3de98b766e11a2567cc784ea6b66b90/%4971d643abdeeeffc046a2d066d632a05b6d860a"

namespace EffectNetwork_Static {
	
// Offsets
	constexpr const static size_t effect = 0x28;
}

namespace BuildingBlock {
	
// Offsets
	constexpr const static size_t grade = 0x2a8;
	
// Functions
	constexpr const static size_t GetBuildMenu = 0x44bf6e0;
	constexpr const static size_t HasUpgradePrivilege = 0x4495310;
	constexpr const static size_t CanAffordUpgrade = 0x44a2270;
}

namespace PlayerWalkMovement {
	constexpr const static size_t ClientInput = 0x5b00180;
}

// obf name: ::%ad420b9a290a06071353915c6f692fb208851e69
#define GameObjectEx_ClassName "%ad420b9a290a06071353915c6f692fb208851e69"

namespace GameObjectEx {
	
// Functions
	constexpr const static size_t ToBaseEntity = 0x485cdf0;
}

namespace UIDeathScreen {
	
// Functions
	constexpr const static size_t SetVisible = 0x247a8f0;
}

// obf name: ::%54005e19a403aea3ecc91dd395c9df23070f99c5
#define BaseScreenShake_Static_ClassName "BaseScreenShake/%54005e19a403aea3ecc91dd395c9df23070f99c5"

namespace BaseScreenShake_Static {
	
// Offsets
	constexpr const static size_t list = 0xe0;
}

namespace FlashbangOverlay {
	
// Offsets
	constexpr const static size_t Instance = 0x40;
	constexpr const static size_t flashLength = 0x48;
}

// obf name: ::%7fe8c9f7c4eaa0312707c2190ff12e22781295c9
#define StringPool_ClassName "%7fe8c9f7c4eaa0312707c2190ff12e22781295c9"

namespace StringPool {
	
// Functions
	constexpr const static size_t Get = 0x47d2b30;
}

// obf name: ::%722279c6ba77d2bb3466b6b56b242544e3b9ed74
#define Network_Networkable_ClassName "%722279c6ba77d2bb3466b6b56b242544e3b9ed74"

namespace Network_Networkable {
	
// Offsets
	constexpr const static size_t ID = 0x48;
}

// obf name: ::%4b7560a57ea3bdf87fe4cadad8eaf838692e0b8e
#define Network_Net_ClassName "%4b7560a57ea3bdf87fe4cadad8eaf838692e0b8e"

namespace Network_Net {
	
// Offsets
	constexpr const static size_t cl = 0x40;
}

// obf name: ::%49b46fcf70b02170fd9da2c588b432fde21544fb
#define Network_Client_ClassName "%49b46fcf70b02170fd9da2c588b432fde21544fb"

namespace Network_Client {
	
// Offsets
	constexpr const static size_t Connection = 0xe8;
	constexpr const static size_t ConnectedPort = 0xf8;
	constexpr const static size_t ConnectedAddress = 0xe0;
}

// obf name: ::%5f92dec0770431eda470f92fa2e24c17509dd6b4
#define Network_BaseNetwork_ClassName "%5f92dec0770431eda470f92fa2e24c17509dd6b4"

namespace Network_BaseNetwork {
	
// Functions
	constexpr const static size_t StartWrite = 0x6b454b0;
}

// obf name: ::%aa0d0be53bf0e718c152d84d493efde511817968
#define Network_SendInfo_ClassName "%aa0d0be53bf0e718c152d84d493efde511817968"

namespace Network_SendInfo {
	
// Offsets
	constexpr const static size_t method = 0x0;
	constexpr const static size_t channel = 0x4;
	constexpr const static size_t priority = 0x8;
	constexpr const static size_t connections = 0x10;
	constexpr const static size_t connection = 0x18;
}

// obf name: ::%03fd289c734c7c30e65a09e4197e8267c1d79ba7
#define Network_NetWrite_ClassName "%03fd289c734c7c30e65a09e4197e8267c1d79ba7"

namespace Network_NetWrite {
	
// Functions
	constexpr const static size_t UInt8 = 0x6add720;
	constexpr const static size_t UInt32 = 0x6ae2c40;
	constexpr const static size_t UInt64 = 0x6ad9590;
	constexpr const static size_t Send = 0x6ada400;
}

namespace LootPanel {
	
// Functions
	constexpr const static size_t get_Container_00 = 0x4033190;
}

namespace UIInventory {
	
// Functions
	constexpr const static size_t Close = 0x542b060;
}

namespace GrowableEntity {
	
// Offsets
	constexpr const static size_t Properties = 0x270;
	constexpr const static size_t State = 0x280;
}

namespace PlantProperties {
	
// Offsets
	constexpr const static size_t stages = 0x28;
}

namespace PlantProperties_Stage {
	
// Offsets
	constexpr const static size_t resources = 0xc;
}

namespace Text {
	
// Offsets
	constexpr const static size_t m_Text = 0xd8;
}

namespace TOD_Sky {
	
// Functions
	constexpr const static size_t get_Instance = 0xb32b50;
	
// Offsets
	constexpr const static size_t Cycle = 0x40;
	constexpr const static size_t Night = 0x60;
	constexpr const static size_t Ambient = 0x98;
}

namespace TOD_NightParameters {
	
// Offsets
	constexpr const static size_t AmbientColor = 0x40;
}

namespace TOD_AmbientParameters {
	
// Offsets
	constexpr const static size_t Saturation = 0x14;
}

namespace UIHUD {
	
// Offsets
	constexpr const static size_t Hunger = 0x28;
}

namespace HudElement {
	
// Offsets
	constexpr const static size_t lastValue = 0x30;
}

namespace UIBelt {
	
// Offsets
	constexpr const static size_t ItemIcons = 0x20;
}

namespace ItemModCompostable {
	
// Offsets
	constexpr const static size_t MaxBaitStack = 0x28;
}

// obf name: ::ResourceRef`1
#define GameObjectRef_ClassName "ResourceRef<UnityEngine/GameObject>"

namespace GameObjectRef {
	
// Offsets
	constexpr const static size_t guid = 0x10;
}

namespace EnvironmentManager {
	
// Functions
	constexpr const static size_t Get = 0x26cdbc0;
}

namespace Translate_Phrase {
	
// Offsets
	constexpr const static size_t english = 0x0;
}

namespace ResourceDispenser_GatherPropertyEntry {
	
// Offsets
	constexpr const static size_t gatherDamage = 0x10;
	constexpr const static size_t destroyFraction = 0x14;
	constexpr const static size_t conditionLost = 0x18;
}

namespace ResourceDispenser_GatherProperties {
	
// Offsets
	constexpr const static size_t Tree = 0x10;
	constexpr const static size_t Ore = 0x18;
	constexpr const static size_t Flesh = 0x20;
}

namespace UIChat {
	
// Offsets
	constexpr const static size_t chatArea = 0x28;
}

namespace OBB {
	
// Functions
	constexpr const static size_t ctor = 0xf5a40000;
	constexpr const static size_t ClosestPoint = 0xf5a40000;
}

namespace PatrolHelicopter {
	
// Offsets
	constexpr const static size_t mainRotor = 0x238;
}

namespace Chainsaw {
	
// Offsets
	constexpr const static size_t ammo = 0x364;
}

// obf name: ::%1d669babe7da59bff89de72d98a95390425910b9
#define CameraUpdateHook_Static_ClassName "CameraUpdateHook/%1d669babe7da59bff89de72d98a95390425910b9"

namespace CameraUpdateHook_Static {
	
// Offsets
	constexpr const static size_t action = 0x70;
}

namespace SteamClientWrapper {
	
// Functions
	constexpr const static size_t GetAvatarTexture = 0x446bd10;
}

