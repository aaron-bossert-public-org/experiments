check write barrier generator identifie for both read and write dependencies.
cannot write multiple times in the same cluster, and cannot read and write in the same cluster.

const auto& ident = cluster -> identifier();


const auto& resource_ident =
	write_dependency.resource() -> write_barrier_generator();

if ( resource_ident.generation == ident.generation &&
	 resource_ident.generator == ident.generator )
{
	LOG_CRITICAL(
		"the same cluster cannot contain multiple write barriers for "
		"the same resource. The write dependencies in this cluster "
		"will be hazardous to each other" );
}
