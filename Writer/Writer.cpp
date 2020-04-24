#include "Writer.h"

#ifdef __linux__ 
using boost::interprocess::shared_memory_object;
#elif _WIN32
using boost::interprocess::windows_shared_memory;
#endif

using boost::interprocess::named_sharable_mutex;
using boost::interprocess::mapped_region;
using boost::interprocess::scoped_lock;

void create_data(flatbuffers::FlatBufferBuilder& builder, unsigned int count) {

	std::vector<flatbuffers::Offset<Gintama::Character>> member_vector;

	auto gin_name = builder.CreateString("Sakata Gintoki");
	auto gin_quote = builder.CreateString("Idiots, in their own idiotic way, have their own idiotic worries.");
	Gintama::CharacterBuilder gintoki(builder);
	gintoki.add_name(gin_name);
	gintoki.add_quote(gin_quote);
	auto gin_offset = gintoki.Finish();
	member_vector.push_back(gin_offset);

	auto shinpachi_name = builder.CreateString("Shimura Shinpachi");
	auto shinpachi_quote = builder.CreateString("This country is done for.");
	Gintama::CharacterBuilder shinpachi(builder);
	shinpachi.add_name(shinpachi_name);
	shinpachi.add_quote(shinpachi_quote);
	auto shinpachi_offset = shinpachi.Finish();
	member_vector.push_back(shinpachi_offset);

	auto kagura_name = builder.CreateString("Kagura");
	auto kagura_quote = builder.CreateString("HAAARLEEEY DAVIDSOOOON!!!");
	Gintama::CharacterBuilder kagura(builder);
	kagura.add_name(kagura_name);
	kagura.add_quote(kagura_quote);
	auto kagura_offset = kagura.Finish();
	member_vector.push_back(kagura_offset);

	auto members = builder.CreateVector(member_vector);

	Gintama::YorozuyaBuilder yorozuya(builder);
	yorozuya.add_index(count);
	yorozuya.add_member(members);

	auto forever_yorozuya = yorozuya.Finish();

	builder.Finish(forever_yorozuya);

	fmt::print("Serialized size: {}\n", builder.GetSize());
}

int main(int argc, char** argv)
{
	flatbuffers::FlatBufferBuilder builder;

	// Create a shared memory object.
#ifdef __linux__ 
	shared_memory_object shm_obj(boost::interprocess::open_or_create,
		"shared_memory",
		boost::interprocess::read_write);

	//Set size
	shm_obj.truncate(1000);

#elif _WIN32
	windows_shared_memory shm_obj(boost::interprocess::open_or_create,
		"shared_memory",
		boost::interprocess::read_write,
		1000
	);
#endif

	// Open or create the named mutex
	named_sharable_mutex mutex(boost::interprocess::open_or_create, "PandoraBox");

	//Map the whole shared memory in this process
	mapped_region region(shm_obj, boost::interprocess::read_write);

	unsigned int count{ 0 };

	while (true) {
		// Introduce new scope so that the mutex is released before sleeping
		{
			create_data(builder, count);

			// Lock the mutex, exclusive lock
			scoped_lock<named_sharable_mutex> lock(mutex);

			// Write the generated flatbuffers to region
			std::memcpy(region.get_address(), builder.GetBufferPointer(), builder.GetSize());
		}

		builder.Clear();
		count++;

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
