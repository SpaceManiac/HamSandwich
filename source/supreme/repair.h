#ifndef REPAIR_H
#define REPAIR_H

#include <vector>

// this table stuff is needed for the complex adjustments that can be made,
// like shifting levels around or pasting/moving/deleting blocks of tiles
class SwapTable
{
	std::vector<int> swapTable;
public:
	explicit SwapTable(int size);
	void Swap(int me, int you);
	void Delete(int me);
	void DeleteBlock(int me, int you); // Inclusive range.
	void SwapBlock(int me, int me2, int you); // Inclusive range.

	int GetSwap(int me) const;
};

// all of these repair functions are used
// when a thing of that type is deleted.  It scratches out
// any reference to the deleted object (or sets it to a default),
// and subtracts 1 from any reference to an object greater than the deleted object.
void RepairCustomSounds(int n);
void RepairTiles(const SwapTable &table);
void RepairItems(int n);
void RepairLevels(const SwapTable &table);

#endif
