

#ifndef BB_CORESORT_H
#define BB_CORESORT_H

namespace bigfx
{

template<typename T>
struct TLess
{
	inline bool operator()(const T& A, const T& B) const { return A < B; }
};

template<typename T>
struct TGreater
{
	inline bool operator()(const T& A, const T& B) const { return B < A; }
};


template<class T, class PREDICATE_CLASS> 
void sort( T* First, const int32 Num, const PREDICATE_CLASS& Predicate )
{
	struct FStack
	{
		T* Min;
		T* Max;
	};

	if( Num < 2 )
	{
		return;
	}
	FStack RecursionStack[32]={{First,First+Num-1}}, Current, Inner;
	for( FStack* StackTop=RecursionStack; StackTop>=RecursionStack; --StackTop )
	{
		Current = *StackTop;
Loop:
		size_t Count = Current.Max - Current.Min + 1;
		if( Count <= 8 )
		{
			// Use simple bubble-sort.
			while( Current.Max > Current.Min )
			{
				T *Max, *Item;
				for( Max=Current.Min, Item=Current.Min+1; Item<=Current.Max; Item++ )
				{
					if( Predicate( *Max, *Item ) )
					{
						Max = Item;
					}
				}
				swap( *Max, *Current.Max-- );
			}
		}
		else
		{
			// Grab middle element so sort doesn't exhibit worst-cast behavior with presorted lists.
			swap( Current.Min[Count/2], Current.Min[0] );

			// Divide list into two halves, one with items <=Current.Min, the other with items >Current.Max.
			Inner.Min = Current.Min;
			Inner.Max = Current.Max+1;
			for( ; ; )
			{
				while( ++Inner.Min<=Current.Max && !Predicate( *Current.Min, *Inner.Min ) );
				while( --Inner.Max> Current.Min && !Predicate( *Inner.Max, *Current.Min ) );
				if( Inner.Min>Inner.Max )
				{
					break;
				}
				swap( *Inner.Min, *Inner.Max );
			}
			swap( *Current.Min, *Inner.Max );

			// Save big half and recurse with small half.
			if( Inner.Max-1-Current.Min >= Current.Max-Inner.Min )
			{
				if( Current.Min+1 < Inner.Max )
				{
					StackTop->Min = Current.Min;
					StackTop->Max = Inner.Max - 1;
					StackTop++;
				}
				if( Current.Max>Inner.Min )
				{
					Current.Min = Inner.Min;
					goto Loop;
				}
			}
			else
			{
				if( Current.Max>Inner.Min )
				{
					StackTop->Min = Inner  .Min;
					StackTop->Max = Current.Max;
					StackTop++;
				}
				if( Current.Min+1<Inner.Max )
				{
					Current.Max = Inner.Max - 1;
					goto Loop;
				}
			}
		}
	}
}

} /* namespace bigfx */

#endif // BB_CORESORT_H

