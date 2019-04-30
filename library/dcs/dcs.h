#pragma once

// Should come with built in locking mechanisms.
// Domain-wide constraint that all locked objects must be accessed through a temp 'lock_ptr'.