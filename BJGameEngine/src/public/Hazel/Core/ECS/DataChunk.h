
// memory page 의 크기가 16384 이다.
// memory page 란, os 에 의해서 할당, 해제 될 수 있는 가장 작은 단위의 메모리이다.
// data chunk 의 크기를 해당 크기로 제한 하면, 각각의 chunk 가 single memory page 에
// 딱 맞게 들어가게 된다.
// 그러면, chunk 를 할당, 해제하는 과정을 더 효과적으로 할 수 있고
// fragmentation 도 막을 수 있다.

// Fragmentation occurs when memory is allocated and deallocated in a way that leaves small, 
// unused gaps of memory between allocated blocks. 
// These gaps can make it difficult to allocate large blocks of memory,
constexpr size_t BLOCK_MEMORY_16K = 16384;

class TestArchetype;

inline constexpr uint64_t hash_64_fnv1a(const char* key, const uint64_t len) {

		uint64_t hash = 0xcbf29ce484222325;
		uint64_t prime = 0x100000001b3;

		for (int i = 0; i < len; ++i) {
			uint8_t value = key[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}

	inline constexpr uint64_t hash_fnv1a(const char* key) {
		uint64_t hash = 0xcbf29ce484222325;
		uint64_t prime = 0x100000001b3;

		int i = 0;
		while (key[i]) {
			uint8_t value = key[i++];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}

	template<typename T, int BucketSize>
	struct HashCache {

		struct Bucket {
			std::array<T, BucketSize> items;
			std::array<size_t, BucketSize> item_hashes;
		};

		HashCache() {
			for (int i = 0; i < 8; ++i) {
				buckets[i] = Bucket{};
			}
		}

		std::array<Bucket, 8> buckets;
	};

	struct MetatypeHash {
		size_t name_hash{ 0 };
		size_t matcher_hash{ 0 };

		bool operator==(const MetatypeHash& other)const {
			return name_hash == other.name_hash;
		}
		template<typename T>
		static constexpr const char* name_detail() {
			return __FUNCSIG__;
		}

		template<typename T>
		static constexpr size_t hash() {

			static_assert(!std::is_reference_v<T>, "dont send references to hash");
			static_assert(!std::is_const_v<T>, "dont send const to hash");
			return hash_fnv1a(name_detail<T>());
		}
	};
	struct Metatype {

		using ConstructorFn = void(void*);
		using DestructorFn = void(void*);

		MetatypeHash hash;

		const char* name{ "none" };
		ConstructorFn* constructor;
		DestructorFn* destructor;
		uint16_t size{ 0 };
		uint16_t align{ 0 };

		// 여기 align 이라는 값은, 
		// type 의 size..? 처럼 보인다.
		bool is_empty() const { return align == 0; };


		template<typename T>
		static constexpr MetatypeHash build_hash() {

			using sanitized = std::remove_const_t<std::remove_reference_t<T>>;

			MetatypeHash hash;
			hash.name_hash = MetatypeHash::hash<sanitized>();
			hash.matcher_hash |= (uint64_t)0x1L << (uint64_t)((hash.name_hash) % 63L);
			return hash;
		};
		template<typename T>
		static constexpr Metatype build() {

			Metatype meta{};
			meta.hash = build_hash<T>();

			if constexpr (std::is_empty_v<T>)
			{
				meta.align = 0;
				meta.size = 0;
			}
			else {
				meta.align = alignof(T);
				meta.size = sizeof(T);
			}

			meta.constructor = [](void* p) {
				new(p) T{};
			};
			meta.destructor = [](void* p) {
				((T*)p)->~T();
			};

			return meta;
		};
	};

	//has stable pointers, use name_hash for it
	static robin_hood::unordered_node_map<uint64_t, Metatype> metatype_cache;

	struct EntityID {
		uint32_t index;
		uint32_t generation;
	};
struct DataChunkHeader {

	// pointer to the signature for this block
    // componentList 라는 구조체 안에, 현재 byte 메모리 DataChunk 중에서
    // 어떤 offset 에 어떤 component 가 있는지에 대한 정보가 담긴 것으로 보인다.
	struct ChunkComponentList* componentList;
	
    // 그리고 ArcheType Class 가 별도로 존재하고
    // 해당 ArcheType 이 어떤 형태로든 DataChunk ? 들을 들고 있는 것 같다.
    struct TestArchetype* ownerArchetype{ nullptr };

    // chunk 들을 연결리스트 형태로 저장하는 것으로 보인다.
	struct DataChunk* prev{ nullptr };
	struct DataChunk* next{ nullptr };

	// 해당 chunk 가 가지고 있는 entity 개수 
	// 해당 크기가 , componentList 의 chunkCapacity 보다 클 수 없다.
	// 실제로, 해당 Chunk 에 entity 를 추가할 때마다 last++ 을 해준다.
	int16_t last{ 0 };
};

// alignas(32) : struct's starting memory address must be multiple of 32
// 왜 32 만큼 align 을 맞추는 것일까 ?

// This alignment requirement is necessary 
// to ensure that the struct can be accessed efficiently by the CPU.
// 왜 ?
// cpu 는, optimal performance 를 위해서
// they prefer data to be located on specific memory addresses aligned to certain boundaries
// 원인 1 . Cache Lines: 
// CPUs have caches, which are smaller, faster memories 
// that hold frequently accessed data. 
// Data is transferred between main memory 
// and the cache in fixed-size chunks called cache lines. 
// If a data structure (like DataChunk) straddles multiple cache lines, 
// accessing it requires fetching data from multiple lines, 
// which is slower than fetching it from a single line. 
// By aligning the struct, you ensure it fits entirely within a single cache line, 
// enabling faster access.
// 자. 아래 storage 를 반드시 첫번째 변수로 둬야 한ㄷ.
// DataChunk 의 시작 주소가, storage 와 동일하게 해야 한다.
struct alignas(32) DataChunk {
    // 실제 데이터 크기는, BLOCK_MEMORY_16K - Header 크기만큼 한다.
	byte storage[BLOCK_MEMORY_16K - sizeof(DataChunkHeader)];
	DataChunkHeader header;
};
static_assert(sizeof(DataChunk) == BLOCK_MEMORY_16K, "chunk size isnt 16kb");

// adv::ChunkComponentList* build_component_list 함수를 통해서 새로운 component_list 를 만든다.
struct ChunkComponentList
{
	struct CmpPair {
		// Component 에 대응되는 Reflection 정보라고 생각하면 된다.
		const Metatype* type;

		// 해당 Component 의 고유한 Hash 정보
		MetatypeHash hash;
		uint32_t chunkOffset;
	};

    // 몇개 set 가 들어가있는가
    // ex) 해당 DataChunk 가 Transform, Move, Name 이라는 3개의 Component Set 를 담는다면
    // 이런 set 가 몇개 들어가 있는가.
	// 다른 말로 하면, 최대 몇개의 entity 정보가 들어갈 수 있는가. 
	int16_t chunkCapacity;

	std::vector<CmpPair> components;
};

template<typename T>
struct ComponentArray {

    ComponentArray() = default;
    ComponentArray(void* pointer, DataChunk* owner) {
        data = (T*)pointer;
        chunkOwner = owner;
    }

    const T& operator[](size_t index) const {
        return data[index];
    }
    T& operator[](size_t index) {
        return data[index];
    }
    bool valid()const {
        return data != nullptr;
    }
    T* begin() {
        return data;
    }
    T* end() {
        return data + chunkOwner->header.last;
    }
    int16_t size() {
        return chunkOwner->header.last;
    }
	// 이렇게 data 변수를 첫번재 멤버 변수로 세팅하면
	// ComponentArray 데이터의 시작주소가 곧 data 가 된다.
	// 다시 말해서, ComponentArray* 는 아래 data 에 해당하는 메모리 주소가 되는 것이다.
	// >> 그러면 data 라는 변수에는 어떤 값이 들어가는 것일까 ?
	// chunk 내에서, component 메모리 시작주소가 들어가게 된다.
	// ex) Chunk 는 [Entity ID, Id..../Transform, Transform, Transform.../Name, NAme, Name...] 이런 형태가 된다.
	// 이때, Transform 첫번째 데이터의 시작주소가 data 라는 변수로 들어오게 되고
	// data[entity id] 를 통해서, 해당 entity id 에 대응되는 component data 를 얻어올 수 있게 되는 것이다.
	// 참고 ) get_entity_component 함수
    T* data{ nullptr };
    DataChunk* chunkOwner{ nullptr };
};



struct Archetype {
    ChunkComponentList* componentList;
    struct ECSWorld* ownerWorld;
    size_t componentHash;
	// (참고) set_chunk_full
	// 해당 ArcheType 이 들고 있는 DataChunk 들 중에서 full 로 찬 DataChunk 가 몇개인가.
    int full_chunks;
    // 일단, ArcheType 은 여러개의 chunk 를 들고 있게 하는 것 같다.
    std::vector<DataChunk*> chunks;
};

// entity 와 관련된 정보를 담은 class 이다.
struct EntityStorage {
	// 해당 entity 가 속한 chunk
    DataChunk* chunk;

	// entity 재활용을 대비하여 generation 이라는 변수를 세팅하낟.
	// 그런데 EntityID 도 generatio 이라는 변수가 있는데,
	// EntityStorage 까지 있어야 하는건가 ?
	// 답변 : is_entity_valid 함수를 보면, 여기 변수와 entityId.generation 이 같아야지만, valid 한 entity 이다.
    uint32_t generation;

	// 해당 chunk 에서 몇번째 idex 에 위치하는가.
    uint16_t chunkIndex;

    bool operator==(const EntityStorage& other) const {
        return chunk == other.chunk && generation == other.generation && chunkIndex == other.chunkIndex;
    }

    bool operator!=(const EntityStorage& other) const{
        return !(other == *this);
    }
};
	namespace adv {
		//forward declarations
		inline int insert_entity_in_chunk(DataChunk* chunk, EntityID EID, bool bInitializeConstructors = true);
		inline EntityID erase_entity_in_chunk(DataChunk* chunk, uint16_t index);
		inline DataChunk* build_chunk(ChunkComponentList* cmpList);


		template<typename... Type>
		struct type_list {};

		template<typename Class, typename Ret, typename... Args>
		type_list<Args...> args(Ret(Class::*)(Args...) const);

		template<typename T>
		static const Metatype* get_metatype() {
			static const Metatype* mt = []() {
				constexpr size_t name_hash = Metatype::build_hash<T>().name_hash;

				auto type = metatype_cache.find(name_hash);
				if (type == metatype_cache.end()) {
					constexpr Metatype newtype = Metatype::build<T>();
					metatype_cache[name_hash] = newtype;
				}
				return &metatype_cache[name_hash];
			}();
			return mt;
		}


		//reorder archetype with the fullness
		inline void set_chunk_full(DataChunk* chunk) {

			Archetype* arch = chunk->header.ownerArchetype;
			if (arch) {
				arch->full_chunks++;

				//do it properly later
				// 해당 Archetype 이 들고 있는 chunk 에 들어갈 수 있는
				// entity 의 최대 개수
				int archsize = arch->componentList->chunkCapacity;

				// std::partition 은, 조건에 따라서 element 들을 재정렬한다.
				// 조건을 만족하는 대상들을 앞에, 만족하지 못하는 대상들을 뒤에 둔다.
				// 즉, full 인 data chunk 들을 앞쪽에 배치하기 위한 코드로 보인다.
				// 그리고 아직 full 이 아니고, 빈 부분이 있는 chunk 들을 뒤쪽에 배치한다.
				std::partition(arch->chunks.begin(), arch->chunks.end(), [archsize](DataChunk* cnk) {
					return cnk->header.last == archsize;
					});
			}


		}
		//reorder archetype with the fullness
		inline void set_chunk_partial(DataChunk* chunk) {

			Archetype* arch = chunk->header.ownerArchetype;

			// archtype 이 들고 있는 chunk 들 중, full 로 차있는 chunk 의 개수를 - 1 해준다.
			arch->full_chunks--;

			//do it properly later
			int archsize = arch->componentList->chunkCapacity;

			// 여기서도 set_chunk_full 함수와 마찬가지로
			// full 여부를 기준으로, chunk 들을 재정렬한다.
			std::partition(arch->chunks.begin(), arch->chunks.end(), [archsize](DataChunk* cnk) {
				return cnk->header.last == archsize;
				});
		}

		inline ChunkComponentList* build_component_list(const Metatype** types, size_t count) {
			ChunkComponentList* list = new ChunkComponentList();

            // component 의 total size 를 계산하고자 한다.
            // 1) EntityID 
            // 2) component 개수 * count
            // 여기서 count 는, type 의 개수이다.
			int compsize = sizeof(EntityID);

            // 자. DataChunk 라는 것 자체가, 여러 Component Data 덩어리가 모여있을 수 있는 녀석이다.
            // ex) 결국 예를 들어, Transform, Move, Name Component 가 하나의 Data Chunk 안에
            // 들어올 수 있다는 것이다.
			for (size_t i = 0; i < count; i++) {

				compsize += types[i]->size;
			}

			size_t availibleStorage = sizeof(DataChunk::storage);

			// 2 less than the real count to account for sizes and give some slack
            // chenk storage 에 fit 할 수 있게끔.
            // ex) 총 size : 100, compSize : 10, itemCount : 8
            // 여기서 itemCount 란, 해당 DataChunk 에
            // 총 (Transform, Move, Name Component) set 가 8개 들어간다는 의미이다.
			size_t itemCount = (availibleStorage / compsize) - 2;

            // chunk storage 에서, 첫번째 component 의 offset 을 계산한다.
            // 최초 offset 은, DataChunkHeader Size + total size of EntityID of all items
            // (질문) 그런데 굳이 DataChunkHeader 만큼을 더 주는 이유가 뭘까 ?
            // 이미 DataChunk  라는 Class 에는, DataChunkHeader 변수가 따로 있는데...
			uint32_t offsets = sizeof(DataChunkHeader);

			// insert_entity_in_chunk 함수 참고
			// DataChunk 구조가 : [entity ID, entity ID...][Transform, Transform..][Move, Move...][Name, Name...]
			// 이렇게 앞쪽에는 entity ID, 그리고 뒤에 Component 정보들을 모아두는 방식으로 관리하는 것으로 보인다.
			offsets += sizeof(EntityID) * itemCount;

			for (size_t i = 0; i < count; i++) {

                // 모든 meta type 정보를 순회한다.
				const Metatype* type = types[i];

                // 만약 Metatype 이 alignment 를 요구한다면
                // 그 align 을 적용하여 offset 을 맞춰준다.
				if (type->align != 0) {
					//align properly
                    // ex) offset : 100, align : 8 -> remainder : 4
                    // oset : 8 - 4 = 4
                    // offset : 100 + 4 == 104
					size_t remainder = offsets % type->align;
					size_t oset = type->align - remainder;
					offsets += oset;
				}

                // 그리고 관련 정보를 CmpPair 라는 형태의 데이터로 담아서
                // components 라는 vector 에 push 한다.
                // (짐작) DataChunk 는 byte memory, 여기서 각 component 의 시작 위치를
                // 담은 CmpPair 라는 정보를 모아서 세팅하는 것으로 보인다.
				// ex) Transform, Name, Move 가 있다고 한다면
				// Chunk 에는 [Transform, Transform...][Name, Name...][Move, Move...] 순서 형태로 저장하겠다는 의미이다.
				list->components.push_back({ type,type->hash,offsets });

                // 마찬가지로 align 을 요구하면
                // itemCount 개수 * size 만큼 offset 을 증가시켜준다.
                // 이를 통해, 다음 type 의 data 들이 DataChunk 에 들어가게 하는 것으로 보인다.
				if (type->align != 0) {
					offsets += type->size * (itemCount);
				}

			}

			// implement proper size handling later
            // offset 이 적정크기 ? DataChunk 의 크기를 넘으면 안된다. 
			assert(offsets <= BLOCK_MEMORY_16K);

			list->chunkCapacity = itemCount;

			return list;
		}






		inline size_t build_signature(const Metatype** types, size_t count) {
			size_t and_hash = 0;
			//for (auto m : types)
			for (int i = 0; i < count; i++)
			{
				//consider if the fancy hash is needed, there is a big slowdown
				//size_t keyhash = ash_64_fnv1a(&types[i]->name_hash, sizeof(size_t));
				//size_t keyhash = types[i]->name_hash;

				and_hash |= types[i]->hash.matcher_hash;
				//and_hash |=(uint64_t)0x1L << (uint64_t)((types[i]->name_hash) % 63L);
			}
			return and_hash;
		}

		inline DataChunk* create_chunk_for_archetype(Archetype* arch) {
            // 새로운 chunk 를 일단 만든다.
			DataChunk* chunk = build_chunk(arch->componentList);

			chunk->header.ownerArchetype = arch;

			arch->chunks.push_back(chunk);
			
            return chunk;
		}
		inline void delete_chunk_from_archetype(DataChunk* chunk) {
			Archetype* owner = chunk->header.ownerArchetype;
			DataChunk* backChunk = owner->chunks.back();

			// 현재 지우고자 하는 chunk 가, 맨 마지막 chunk 가 아니라면
			if (backChunk != chunk) {
				for (int i = 0; i < owner->chunks.size(); i++) {
					if (owner->chunks[i] == chunk) {
						// 현재 지우는 chunk 위치에, 가장 마지막 chunk 를 세팅한다.
						owner->chunks[i] = backChunk;
					}
				}
			}
			// 그리고 마지막 chunk * 정보는 지워준다.
			owner->chunks.pop_back();

			// 실제 chunk data 를 메모리 해제 한다.
			delete chunk;
			
		}
		inline bool compare_metatypes(const Metatype* A, const Metatype* B) {
			//return A->name_hash < B->name_hash;
			return A < B;
		}
		inline size_t join_metatypes(const Metatype* ATypes[], size_t Acount, const Metatype* BTypes[], size_t Bcount, const Metatype* output[]) {

			const Metatype** AEnd = ATypes + Acount;
			const Metatype** BEnd = BTypes + Bcount;

			const Metatype** A = ATypes;
			const Metatype** B = BTypes;
			const Metatype** C = output;

			while (true)
			{
				if (A == AEnd) {
					std::copy(B, BEnd, C);
				}
				if (B == BEnd) {
					std::copy(A, AEnd, C);
				}

				if (*A < *B) { *C = *A; ++A; }
				else if (*B < *A) { *C = *B; ++B; }
				else { *C = *A; ++A; ++B; }
				++C;
			}

			return C - output;
		}

		inline void sort_metatypes(const Metatype** types, size_t count) {
			std::sort(types, types + count, [](const Metatype* A, const Metatype* B) {
				return compare_metatypes(A, B);
				});
		}
		inline bool is_sorted(const Metatype** types, size_t count) {
			for (int i = 0; i < count - 1; i++) {
				if (types[i] > types[i + 1]) {
					return false;
				}
			}
			return true;
		}

		// types : 해당 Archetype 을 구성하고 있는 component type 목록들
		inline Archetype* find_or_create_archetype(ECSWorld* world, const Metatype** types, size_t count) {
			const Metatype* temporalMetatypeArray[32];

			// 하나의 component 가 32 개 이상을 들고 있지 않도록 한다.
			assert(count < 32);

			const Metatype** typelist;

			if (false) {//!is_sorted(types, count)) {
				for (int i = 0; i < count; i++) {
					temporalMetatypeArray[i] = types[i];

				}
				sort_metatypes(temporalMetatypeArray, count);
				typelist = temporalMetatypeArray;
			}
			else {
				typelist = types;
			}

			// MetaType 의 matcher_hash 들을 | 연산자를 통해, 합친다
			// 이를 통해 matcher 를 만들어낸다.
			const uint64_t matcher = build_signature(typelist, count);

			//try in the hashmap
			auto iter = world->archetype_signature_map.find(matcher);

			// 기존 ECSWorld.archetype_signature_map 에 해당 matcher 로 된 Archetype 목록을 찾아온다.
			// 즉, 같은 matcher 를 지닌 archetype 들이 존재할 수 잇다는 의미이다.
			if (iter != world->archetype_signature_map.end()) {

				auto& archvec = iter->second;//world->archetype_signature_map[matcher];
				for (int i = 0; i < archvec.size(); i++) {

					auto componentList = archvec[i]->componentList;
					int ccount = componentList->components.size();
					if (ccount == count) {
						for (int j = 0; j < ccount; j++) {

							if (componentList->components[j].type != typelist[j])
							{
								//mismatch, inmediately continue
								goto contA;
							}
						}

						//everything matched. Found. Return inmediately
						return archvec[i];
					}

				contA:;
				}
			}

			// 기존에 존재하는 Archetype 이 없기 때문에, 새로운 Archetype 을 만든다.
			//not found, create a new one
			Archetype* newArch = new Archetype();

			newArch->full_chunks = 0;
			newArch->componentList = build_component_list(typelist, count);
			newArch->componentHash = matcher;
			newArch->ownerWorld = world;
			world->archetypes.push_back(newArch);
			world->archetypeSignatures.push_back(matcher);
			world->archetype_signature_map[matcher].push_back(newArch);

			//we want archs to allways have 1 chunk at least, create initial
			create_chunk_for_archetype(newArch);

			return newArch;
		}


		inline EntityID allocate_entity(ECSWorld* world) {
			// 일단 새로운 Entity ID 를 만든다.
			EntityID newID;
			if (world->dead_entities == 0) {
				// 재활용할 entity 가 하나도 없다면
				int index = world->entities.size();

				// 아예 새롭게 entity 정보를 만든다.
				EntityStorage newStorage;
				newStorage.chunk = nullptr;
				newStorage.chunkIndex = 0;
				newStorage.generation = 1;

				world->entities.push_back(newStorage);

				newID.generation = 1;
				newID.index = index;
			}
			else {
				// entity 를 재활용할 것이다.
				int index = world->deletedEntities.back();
				world->deletedEntities.pop_back();

				// 일단 EntityStorage 의 generation 변수를 ++ 해준다.
				world->entities[index].generation++;

				// 그 정보를 EntityID 에도 세팅해준다.
				newID.generation = world->entities[index].generation;
				newID.index = index;
				world->dead_entities--;
			}


			world->live_entities++;
			return newID;
		}

		inline bool is_entity_valid(ECSWorld* world, EntityID id) {
			//index check
			if (world->entities.size() > id.index&& id.index >= 0) {

				//generation check
				if (id.generation != 0 && world->entities[id.index].generation == id.generation)
				{
					return true;
				}
			}
			return false;
		}
		inline void deallocate_entity(ECSWorld* world, EntityID id) {

			//todo add valid check
			world->deletedEntities.push_back(id.index);
			world->entities[id.index].generation++;
			world->entities[id.index].chunk = nullptr;
			world->entities[id.index].chunkIndex = 0;

			world->live_entities--;
			world->dead_entities++;
		}

		inline void destroy_entity(ECSWorld* world, EntityID id) {
			assert(is_entity_valid(world, id));
			erase_entity_in_chunk(world->entities[id.index].chunk, world->entities[id.index].chunkIndex);
			deallocate_entity(world, id);
		}
		inline DataChunk* find_free_chunk(Archetype* arch) {
			DataChunk* targetChunk = nullptr;
			
			// 만약 archetype 에 chunk 가 하나도 없다면
			// chunk 를 새로 추가해준다.
			if (arch->chunks.size() == 0) {
				targetChunk = create_chunk_for_archetype(arch);
			}
			else {
				// 일단 가장 뒤에 있는 chunk 를 가져온다.
				// 왜냐하면 우리는 set_chunk_full 함수 등을 통해 매번
				// full 인 chunk 는, 앞쪽으로, full 이 아닌 chunk 는 뒤쪽으로 배치하기 때문이다.
				targetChunk = arch->chunks[arch->chunks.size() - 1];

				// chunk is full, create a new one
				// 만약 뒤쪽에 있는 대상 조차도 full 이라면
				// 아예 새로운 chunk 를 만들어야 한다.
				if (targetChunk->header.last == arch->componentList->chunkCapacity) {
					targetChunk = create_chunk_for_archetype(arch);
				}
			}
			return targetChunk;
		}


		inline void move_entity_to_archetype(Archetype* newarch, EntityID id, bool bInitializeConstructors = true) {

			//insert into new chunk
			// 해당 entity 가 속한 기존의 chunk 정보
			DataChunk* oldChunk = newarch->ownerWorld->entities[id.index].chunk;
			// 현재 새로운 archetype 에 들어갈 새로운 chunk 정보
			DataChunk* newChunk = find_free_chunk(newarch);

			// 일단 새로운 chunk 에 해당 entity 를 넣는다.
			int newindex = insert_entity_in_chunk(newChunk, id, bInitializeConstructors);

			// entity 가 기존에 속했던 chunk 에서, 몇번째 idx 에 았었는가.
			int oldindex = newarch->ownerWorld->entities[id.index].chunkIndex;

			// 기존 component 목록 개수
			int oldNcomps = oldChunk->header.componentList->components.size();
			// 새로운 component 목록 개수
			int newNcomps = newChunk->header.componentList->components.size();

			auto& oldClist = oldChunk->header.componentList;
			auto& newClist = newChunk->header.componentList;

			// copy all data from old chunk into new chunk
			// bad iteration, fix later
			// 각 Component Type 별로 Merge struct 정보가 세팅될 것이다.
			struct Merge {
				// 해당 type 의 size 는 얼마인가
				int msize;
				// 현재 component type 이 기존의 chunk 내 여러 type 들 중에서, 몇번째 type 이었는가?
				int idxOld;
				// 현재 component type 이 새로운 chunk 내 여러 type 들 중에서, 몇번째 type 인가.?
				int idxNew;
			};
			
			int mergcount = 0;

			// 하나의 chunk 가 가질 수 있는 최대 component 개수가 31개 이다.
			Merge mergarray[32];

			// 자. 이제 기존 old chunk 데이터들을 새로운 chunk 쪽으로 옮겨야 한다.
			// 기존 old chunk 내에 있던 type 들을 순회한다.
			for (int i = 0; i < oldNcomps; i++) {
				const Metatype* mtCp1 = oldClist->components[i].type;
				// 해당 type 의 size 가 존재한다면
				// 뭔가 유효한 type ? 인지 아닌지를 나타내는 변수 처럼 보이기도 한다.
				if (!mtCp1->is_empty()) {
					for (int j = 0; j < newNcomps; j++) {
						const Metatype* mtCp2 = newClist->components[j].type;

						// pointers are stable
						// newChunk 에서 같은 type 을 찾는다.
						if (mtCp2 == mtCp1) {
							mergarray[mergcount].idxNew = j;
							mergarray[mergcount].idxOld = i;
							mergarray[mergcount].msize = mtCp1->size;
							mergcount++;
							// break ; -> 이미 찾았으니까 여기서 break 해도 되지 않을까 ?
						}
					}
				}
			}

			// 각 component 정보를 돌면서, 기존 chunk 에서
			// 새로운 chunk 로 데이터를 "복사" 해준다.
			// (참고) 여기서 그냥 memcpy 가 아니라
			// move 를 해주면 안되는 건가 ? 어차피 old chunk 에서는 해당 component type 정보를 사용하지도 않을텐데
			for (int i = 0; i < mergcount; i++) {
				//const Metatype* mtCp1 = mergarray[i].mtype;

				//pointer for old location in old chunk
				void* ptrOld = (void*)((byte*)oldChunk + oldClist->components[mergarray[i].idxOld].chunkOffset + (mergarray[i].msize * oldindex));

				//pointer for new location in new chunk
				void* ptrNew = (void*)((byte*)newChunk + newClist->components[mergarray[i].idxNew].chunkOffset + (mergarray[i].msize * newindex));

				//memcopy component data from old to new
				memcpy(ptrNew, ptrOld, mergarray[i].msize);
			}

			//delete entity from old chunk
			erase_entity_in_chunk(oldChunk, oldindex);

			//assign entity chunk data
			newarch->ownerWorld->entities[id.index].chunk = newChunk;
			newarch->ownerWorld->entities[id.index].chunkIndex = newindex;
		}
		inline void set_entity_archetype(Archetype* arch, EntityID id) {

			// if chunk is null, we are a empty entity
			// 만약 아직 해당 entity 에 할당된 chunk 가 없다면
			// Archetype 이 가지고 있는 chunk 중에서, 빈 부분에 Entity Data 를 할당해줘야 한다.
			if (arch->ownerWorld->entities[id.index].chunk == nullptr) {

				DataChunk* targetChunk = find_free_chunk(arch);

				int index = insert_entity_in_chunk(targetChunk, id);
				arch->ownerWorld->entities[id.index].chunk = targetChunk;
				arch->ownerWorld->entities[id.index].chunkIndex = index;
			}
			else {
				move_entity_to_archetype(arch, id, false);
			}
		}
		inline EntityID create_entity_with_archetype(Archetype* arch) {
			ECSWorld* world = arch->ownerWorld;

			// 새로운 Entity 를 만들거나 ,혹은 entity 를 재활용한다.
			EntityID newID = allocate_entity(world);

			set_entity_archetype(arch, newID);

			return newID;
		}
		inline Archetype* get_entity_archetype(ECSWorld* world, EntityID id)
		{
			assert(is_entity_valid(world, id));

			return world->entities[id.index].chunk->header.ownerArchetype;
		}


		template<typename C>
		bool has_component(ECSWorld* world, EntityID id);

		template<typename C>
		C& get_entity_component(ECSWorld* world, EntityID id);

		template<typename C>
		void add_component_to_entity(ECSWorld* world, EntityID id, C&& comp);

		template<typename C>
		void remove_component_from_entity(ECSWorld* world, EntityID id);

		template<typename F>
		void iterate_matching_archetypes(ECSWorld* world, const Query& query, F&& function) {

			for (int i = 0; i < world->archetypeSignatures.size(); i++)
			{
				//if there is a good match, doing an and not be 0
				uint64_t includeTest = world->archetypeSignatures[i] & query.require_matcher;

				//implement later
				uint64_t excludeTest = world->archetypeSignatures[i] & query.exclude_matcher;
				if (includeTest != 0) {

					auto componentList = world->archetypes[i]->componentList;

					//might match an excluded component, check here
					if (excludeTest != 0) {

						bool invalid = false;
						//dumb algo, optimize later					
						for (int mtA = 0; mtA < query.exclude_comps.size(); mtA++) {

							for (auto cmp : componentList->components) {

								if (cmp.type->hash == query.exclude_comps[mtA]) {
									//any check and we out
									invalid = true;
									break;
								}
							}
							if (invalid) {
								break;
							}
						}
						if (invalid) {
							continue;
						}
					}

					//dumb algo, optimize later
					int matches = 0;
					for (int mtA = 0; mtA < query.require_comps.size(); mtA++) {

						for (auto cmp : componentList->components) {

							if (cmp.type->hash == query.require_comps[mtA]) {
								matches++;
								break;
							}
						}
					}
					//all perfect
					if (matches == query.require_comps.size()) {

						function(world->archetypes[i]);
					}
				}


			}

		}



		template<typename C>
		C& get_entity_component(ECSWorld* world, EntityID id)
		{

			EntityStorage& storage = world->entities[id.index];

			auto acrray = get_chunk_array<C>(storage.chunk);
			assert(acrray.chunkOwner != nullptr);
			return acrray[storage.chunkIndex];
		}


		template<typename C>
		bool has_component(ECSWorld* world, EntityID id)
		{
			EntityStorage& storage = world->entities[id.index];

			auto acrray = get_chunk_array<C>(storage.chunk);
			return acrray.chunkOwner != nullptr;
		}
		template<typename C>
		void add_component_to_entity(ECSWorld* world, EntityID id)
		{
			const Metatype* temporalMetatypeArray[32];

			const Metatype* type = get_metatype<C>();


			Archetype* oldarch = get_entity_archetype(world, id);
			ChunkComponentList* oldlist = oldarch->componentList;
			bool typeFound = false;
			int lenght = oldlist->components.size();
			for (int i = 0; i < oldlist->components.size(); i++) {
				temporalMetatypeArray[i] = oldlist->components[i].type;

				//the pointers for metatypes are allways fully stable
				if (temporalMetatypeArray[i] == type) {
					typeFound = true;
				}
			}

			Archetype* newArch = oldarch;
			if (!typeFound) {

				temporalMetatypeArray[lenght] = type;
				sort_metatypes(temporalMetatypeArray, lenght + 1);
				lenght++;


				newArch = find_or_create_archetype(world, temporalMetatypeArray, lenght);



				set_entity_archetype(newArch, id);
			}

		}
		template<typename C>
		void add_component_to_entity(ECSWorld* world, EntityID id, C& comp)
		{
			const Metatype* type = get_metatype<C>();

			add_component_to_entity<C>(world, id);


			//optimize later
			if (!type->is_empty()) {
				get_entity_component<C>(world, id) = comp;
			}
		}

		template<typename C>
		void remove_component_from_entity(ECSWorld* world, EntityID id)
		{
			const Metatype* temporalMetatypeArray[32];

			// 그냥 관련 reflection type 정보를 얻어오는 것으로 보인다.
			const Metatype* type = get_metatype<C>();

			// 해당 Entity Id 에 대응되는 EntityStorage 를 참고하여, 현재 Entity 가 속한
			// Archetype 을 가져온다. 
			Archetype* oldarch = get_entity_archetype(world, id);
			ChunkComponentList* oldlist = oldarch->componentList;
			bool typeFound = false;

			// 해당 archetype 이 들고 있는 component 목록의 개수
			int lenght = oldlist->components.size();

			for (int i = 0; i < lenght; i++) {
				temporalMetatypeArray[i] = oldlist->components[i].type;
				// 현재 Archetype 이 가지고 있는 component 목록 중에서
				// 지우고자 하는 type 을 찾았다면. typeFound 변수를 true 로 세팅
				if (temporalMetatypeArray[i] == type) {

					typeFound = true;

					//swap last
					// 그리고 지금 찾은 type 위치에, 마지막 type 위치 정보를 세팅한다.
					temporalMetatypeArray[i] = oldlist->components[lenght - 1].type;
				}
			}

			Archetype* newArch = oldarch;

			// 만약 지우고자 하는 component type 을
			// archetype 안에서 찾았다면
			if (typeFound) {

				lenght--;

				// hash 값 기준으로, type 정보들을 재정렬한다.
				sort_metatypes(temporalMetatypeArray, lenght);

				// 새로운 component 조합 ? 에 대응되는 Archetype 을 찾는다.
				newArch = find_or_create_archetype(world, temporalMetatypeArray, lenght);

				// 새로운 ? 혹은 기존에 존재하던 archetype 에, entity 를 넣어준다.
				set_entity_archetype(newArch, id);
			}
		}


		//template<typename A, typename B, typename C, typename D, typename Func>
		//void entity_chunk_iterate(DataChunk* chnk, Func&& function) {
		//
		//	auto array0 = get_chunk_array<A>(chnk);
		//	auto array1 = get_chunk_array<B>(chnk);
		//	auto array2 = get_chunk_array<C>(chnk);
		//	auto array3 = get_chunk_array<D>(chnk);
		//
		//	assert(array0.chunkOwner == chnk);
		//	assert(array1.chunkOwner == chnk);
		//	assert(array2.chunkOwner == chnk);
		//	assert(array3.chunkOwner == chnk);
		//	for (int i = chnk->header.last - 1; i >= 0; i--) {
		//		function(array0[i], array1[i], array2[i], array3[i]);
		//	}
		//}

		//by skypjack
		template<typename... Args, typename Func>
		void entity_chunk_iterate(DataChunk* chnk, Func&& function) {
			auto tup = std::make_tuple(get_chunk_array<Args>(chnk)...);
#ifndef NDEBUG
			(assert(std::get<decltype(get_chunk_array<Args>(chnk))>(tup).chunkOwner == chnk), ...);
#endif

			for (int i = chnk->header.last - 1; i >= 0; i--) {
				function(std::get<decltype(get_chunk_array<Args>(chnk))>(tup)[i]...);
			}
		}


		template<typename ...Args, typename Func>
		void unpack_chunk(type_list<Args...> types, DataChunk* chunk, Func&& function) {
			entity_chunk_iterate<Args...>(chunk, function);
		}
		template<typename ...Args>
		Query& unpack_querywith(type_list<Args...> types, Query& query) {
			return query.with<Args...>();
		}





		inline int insert_entity_in_chunk(DataChunk* chunk, EntityID EID, bool bInitializeConstructors) {
			int index = -1;

			ChunkComponentList* cmpList = chunk->header.componentList;

			// 만약 해당 chunk 에 아직 entity 를 넣을 공간이 존재한다면
			if (chunk->header.last < cmpList->chunkCapacity) {

				index = chunk->header.last;

				// 해당 chunk 에 들어있는 entity 개수 정보를 +1 해준다.
				chunk->header.last++;

				if (bInitializeConstructors) {
					//initialize component
					for (auto& cmp : cmpList->components) {
						const Metatype* mtype = cmp.type;

						if (!mtype->is_empty()) {
							void* ptr = (void*)((byte*)chunk + cmp.chunkOffset + (mtype->size * index));

							mtype->constructor(ptr);
						}
					}
				}


				//insert eid
				EntityID* eidptr = ((EntityID*)chunk);
				eidptr[index] = EID;

				//if full, reorder it on archetype
				// 만약 현재 chunk 가 full 이라면, 해당 chunk 를 들고 있는 archetype 의 chunk 들을
				// full 여부에 따라 재정렬한다.
				if (chunk->header.last == cmpList->chunkCapacity) {
					set_chunk_full(chunk);
				}
			}

			return index;
		}

		//returns ID of the moved entity
		inline EntityID erase_entity_in_chunk(DataChunk* chunk, uint16_t index) {

			ChunkComponentList* cmpList = chunk->header.componentList;

			bool bWasFull = chunk->header.last == cmpList->chunkCapacity;
			assert(chunk->header.last > index);

			// 기존에 chunk 에 들어있던 entity 개수가 1보다크고, 현재 지우는 entity 가 마지막 entity 가 아닌 경우
			bool bPop = chunk->header.last > 1 && index != (chunk->header.last - 1);

			// popIndex 는 맨 마지막 idx 를 의미한다.
			int popIndex = chunk->header.last - 1;

			// entity 개수를 지워준다.
			// 해당 chunk 가 들고 있는 entity 개수를 줄여주는 개념이다.
			chunk->header.last--;

			// clear and pop last
			// 각 component 정보들을 순회한다.
			for (auto& cmp : cmpList->components) {
				const Metatype* mtype = cmp.type;

				// 해당 entity 에 대응되는 component 메모리에 접근한다.
				if (!mtype->is_empty()) {
					void* ptr = (void*)((byte*)chunk + cmp.chunkOffset + (mtype->size * index));

					// 소멸자를 호출한다.
					mtype->destructor(ptr);

					if (bPop) {
						void* ptrPop = (void*)((byte*)chunk + cmp.chunkOffset + (mtype->size * popIndex));

						// 맨 마지막에 있던 데이터 정보를, 현재 지운 메모리 위치로 복사해준다.
						memcpy(ptr, ptrPop, mtype->size);
					}
				}
			}

			// 현재 내가 지운 entity 에 대한 메모리도 쓰레기 ? 초기값? 으로 세팅해준다.
			EntityID* eidptr = ((EntityID*)chunk);
			eidptr[index] = EntityID{};


			// 만약 chunk 에 데이터가 더이상 들어있지 않다면
			// 해당 chunk 를 들고 있는 archetype 에서, 해당 chunk 정보를 지운다.
			if (chunk->header.last == 0) {
				delete_chunk_from_archetype(chunk);
			}
			else if (bWasFull) {
				set_chunk_partial(chunk);
			}

			if (bPop) {	
				// 만약 중간 entity 를 지운 것이라면, 일단 entity Storage 에서, 해당 entity 와 관련된 정보를 update 한다.
				chunk->header.ownerArchetype->ownerWorld->entities[eidptr[popIndex].index].chunkIndex = index;
				// 그리고 현재 지운 entity 위치에, 가장 마지막에 위치했던 entity 정보를 세팅한다.
				eidptr[index] = eidptr[popIndex];

				return eidptr[index];
			}
			else {
				// 그게 아니라면 그냥 empty entity ID 를 리턴한다.
				return EntityID{};
			}
		}

		inline DataChunk* build_chunk(ChunkComponentList* cmpList) {

            // 새로운 DataChunk 를 만든다.
			// 해당 DataChunk 들을 메모리 풀 형태로 관리하는 것도 최적화의 방법이 될 수 있을 것 같다.
			DataChunk* chunk = new DataChunk();
			chunk->header.last = 0;
			chunk->header.componentList = cmpList;

			return chunk;
		}
	}

struct ECSWorld {
	std::vector<EntityStorage> entities;
	std::vector<uint32_t> deletedEntities;

    // 아래의 자료구조는 내 맘대로 변경해도 될 것 같다.
    // ECSWorld 생성자를 보면, nullArch 를 만들고 나서
    // 아래 자료구조 key 로 '0' 을 세팅한다.
    // 1) 여기서 '0' 이라는 것은...'0'개 set 가 들어있는 ArcheType들..이라는 의미인가?
	// 2) find_or_create_archetype 함수를 보면, component 의 hash 값들을 합쳐서 matcher 라는 uint64_t 를 만든다.
	//    즉, key 는, hash 조합에 해당하는 값으로 보인다.
	robin_hood::unordered_flat_map<uint64_t, std::vector<Archetype*>> archetype_signature_map{};
	robin_hood::unordered_flat_map<uint64_t, Archetype*> archetype_map{};
	
    // 여러개의 archetype들을 vector 형태로 관리하는 것으로 보인다.
    // (ECSWorld 생성자에서는 nullArche 를 만든다.)
    std::vector<Archetype*> archetypes;
	//unique archetype hashes
	std::vector<size_t> archetypeHashes;
	//bytemask hash for checking
	// archetype_signature_map 의 key 값들을, 또 다시 vector 형태로 저장한다.
	std::vector<size_t> archetypeSignatures;

	robin_hood::unordered_flat_map<uint64_t, void*> singleton_map{};

	// 현재 사용중인 entity 개수
	int live_entities{ 0 };
	// 이미 죽어서 재활용할 수 있는 entity 개수
	int dead_entities{ 0 };
	inline ECSWorld();

	~ECSWorld()
	{
		for (Archetype* arch : archetypes)
		{
			for (DataChunk* chunk : arch->chunks) {
				delete chunk;
			}
			delete arch;
		}
	};
	//needs push_back(DataChunk*) to work, returns number
	template<typename Container>
	int gather_chunks(Query& query, Container& container);

	template<typename Func>
	void for_each(Query& query, Func&& function);

	template<typename Func>
	void for_each(Func&& function);

	template<typename C>
	void add_component(EntityID id, C& comp);
	template<typename C>
	void add_component(EntityID id);

	template<typename C>
	void remove_component(EntityID id);

	template<typename C>
	bool has_component(EntityID id);

	template<typename C>
	C& get_component(EntityID id);

	template<typename C>
	C* set_singleton();
	template<typename C>
	C* set_singleton(C&& singleton);

	template<typename C>
	C* get_singleton();

	template<typename ... Comps>
	inline EntityID new_entity();

	inline void destroy(EntityID eid);

    // 맨 처음 ECSWorld 생성자에서 세팅해준
    // nullArch 를 리턴해주는 함수
	Archetype* get_empty_archetype() { return archetypes[0]; };
};

	template<typename C>
	struct CachedRef
	{
		C* get_from(ECSWorld* world, EntityID target);

		C* pointer;
		EntityStorage storage;
	};

	

	template<typename C>
	C* CachedRef<C>::get_from(ECSWorld* world, EntityID target)
	{	
		if (world->entities[target.index] != storage) {		
			pointer = &world->get_component<C>(target);
			storage = world->entities[target.index];
		}
		return pointer;		
	}

	template<typename T>
	inline auto get_chunk_array(DataChunk* chunk) {

		using ActualT = ::std::remove_reference_t<T>;

		// 만약 인자로 들어온 T 가 EntityID type 이라면
		if constexpr (std::is_same<ActualT, EntityID>::value)
		{
			// chunk 시작 주소를 EntityID 로 변경한다.
			EntityID* ptr = ((EntityID*)chunk);
			return ComponentArray<EntityID>(ptr, chunk);
		}
		else {
			// 그외의 type 이라면
			constexpr MetatypeHash hash = Metatype::build_hash<ActualT>();

			// component 목록을 돌면서
			// 인자로 들어온 type 에 해당하는 component 가 있는지 검사한다.
			for (auto cmp : chunk->header.componentList->components) 
			{
				if (cmp.hash == hash)
				{
					// 만약 현재 인자로 들어온 type 에 해당하는 component 를
					// chunk 에서 찾았다면
					// 해당 chunk 내에서, 해당 component 의 메모리 시작점을 ptr 로 세팅한다.
					void* ptr = (void*)((byte*)chunk + cmp.chunkOffset);
					
					return ComponentArray<ActualT>(ptr, chunk);
				}
			}
			

			return ComponentArray<ActualT>();
		}
	}

	inline ECSWorld::ECSWorld()
	{
		Archetype* nullArch = new Archetype();

		nullArch->full_chunks = 0;
		nullArch->componentList = adv::build_component_list(nullptr, 0);
		nullArch->componentHash = 0;
		nullArch->ownerWorld = this;

		archetypes.push_back(nullArch);

		archetypeSignatures.push_back(0);

		archetype_signature_map[0].push_back(nullArch);

		//we want archs to allways have 1 chunk at least, create initial
		adv::create_chunk_for_archetype(nullArch);
	}

	template<typename Container>
	int ECSWorld::gather_chunks(Query& query, Container& container)
	{
		int count = 0;
		adv::iterate_matching_archetypes(this, query, [&](Archetype* arch) {

			for (auto chnk : arch->chunks) {
				count++;
				container.push_back(chnk);
			}
			});
		return count;
	}

	inline void ECSWorld::destroy(EntityID eid)
	{
		adv::destroy_entity(this, eid);
	}

	template<typename Func>
	void decs::ECSWorld::for_each(Query& query, Func&& function)
	{
		using params = decltype(adv::args(&Func::operator()));

		adv::iterate_matching_archetypes(this, query, [&](Archetype* arch) {

			for (auto chnk : arch->chunks) {

				adv::unpack_chunk(params{}, chnk, function);
			}
			});
	}
	template<typename Func>
	void decs::ECSWorld::for_each(Func&& function)
	{
		using params = decltype(adv::args(&Func::operator()));

		Query query;
		adv::unpack_querywith(params{}, query).build();

		for_each<Func>(query, std::move(function));
	}

	template<typename C>
	inline void ECSWorld::add_component(EntityID id, C& comp)
	{
		adv::add_component_to_entity<C>(this, id, comp);
	}

	template<typename C>
	void ECSWorld::add_component(EntityID id)
	{
		adv::add_component_to_entity<C>(this, id);
	}

	template<typename C>
	inline void ECSWorld::remove_component(EntityID id)
	{
		adv::remove_component_from_entity<C>(this, id);
	}


	template<typename C>
	bool ECSWorld::has_component(EntityID id)
	{
		return adv::has_component<C>(this, id);
	}

	template<typename C>
	C& ECSWorld::get_component(EntityID id)
	{
		return adv::get_entity_component<C>(this, id);
	}

	template<typename C>
	inline C* ECSWorld::set_singleton()
	{
		return set_singleton<C>(C{});
	}

	template<typename C>
	inline C* ECSWorld::set_singleton(C&& singleton)
	{
		constexpr MetatypeHash type = Metatype::build_hash<C>();

		C* old_singleton = get_singleton<C>();
		if (old_singleton) {
			*old_singleton = singleton;
			return old_singleton;
		}
		else {

			C* new_singleton = new C(singleton);
			singleton_map[type.name_hash] = (void*)new_singleton;
			return new_singleton;
		}

	}

	template<typename C>
	inline C* ECSWorld::get_singleton()
	{
		constexpr MetatypeHash type = Metatype::build_hash<C>();

		auto lookup = singleton_map.find(type.name_hash);
		if (lookup != singleton_map.end()) {
			return (C*)singleton_map[type.name_hash];
		}
		else {
			return nullptr;
		}
	}

	template<typename ...Comps>
	inline EntityID decs::ECSWorld::new_entity()
	{
		Archetype* arch = nullptr;
		//empty component list will use the hardcoded null archetype
		if constexpr (sizeof...(Comps) != 0) {
			static const Metatype* types[] = { adv::get_metatype<Comps>()... };
			constexpr size_t num = (sizeof(types) / sizeof(*types));

			adv::sort_metatypes(types, num);
			arch = adv::find_or_create_archetype(this, types, num);
		}
		else {
			arch = get_empty_archetype();
		}

		return adv::create_entity_with_archetype(arch);
	}
}
