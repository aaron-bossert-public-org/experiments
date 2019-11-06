
#include <vector>

namespace igpu
{
	class vulkan_dependency;

	class vulkan_agent
	{
	public:
		~vulkan_agent();

		void resource_moved( vulkan_buffer& );

		void resource_reallocated( vulkan_image& );

	private:
		// each agent references a dependency for each resource it writes to,
		// and each resource that is hazardous to read from (because the
		// resource has been modified by another agent). if an agent reads and
		// writes the same resource, it just considers it a write resource for
		// these purposes

		// the write dependency list does not change during agent processing.

		// the read hazard dependency will become empty after agent processing.
		// and will be repopulated if its input resources are modified by
		// another agent.

		// when an agent is processed it adds required barriers for all of its
		// write dependencies, it then tells those dependencies to notifiy all
		// read agents connected to them that they should be added to their
		// hazard lists

		// when an agent is processed it adds the required barriers for all of
		// its read hazards. to reduce redundant barriers agents should check
		// current barrier state of of read hazards to see if another read agent
		// has added a barrier that will work. (the barrier bit flag logic here
		// is has many complexities)

		// when an agent changes the layout of an image dependency, that image
		// notifies all connected read agents that the dependency is a hazard.

		// when an agent recieves a request to add add a dependency as a hazard,
		// it should check to see if that dependency has its 'is hazard' flag
		// set to avoid adding the same dependency to the hazard list multiple
		// times

		// when an agent is processing its hazards it should ignore any incoming
		// requests, because these would be recursive requests made from the
		// agent itself.


		// when an agent changes the layout of a resource, it notifies that
		// resource that all of its other agents should treat it as a read
		// dependency.

		// this resource could result in the resource being added to
		// multiple read hazard lists that all essentialy want the same
		// barier in place.

		// when an image layout transition is required, all read
		// dependencies need to be notified, with the exception of the
		// current one. current barrier state should be checked to see if a
		// pipeline barrier is necessary. when an agent goes to execute, it
		// checks its read hazards list (which is usually empty) and check
		// to see which barriers are required. on the same resources will be
		// added as hazards to those agents, triggering a barrier before
		// their next execution.

		bool _is_executing;
		std::vector< vulkan_dependency > _read_hazard;
		std::vector< vulkan_dependency > _write;
	};
}
