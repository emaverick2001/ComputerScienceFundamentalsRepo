/*
 * Assembly language function implementations
 */

#define MAX_WORDLEN 63

/*
 * Offsets for the fields of the struct WordEntry data type
 * (Note that there is 4 bytes of padding between the count and next
 * fields.)
 */
#define WORDENTRY_WORD_OFFSET   (0)
#define WORDENTRY_COUNT_OFFSET  (MAX_WORDLEN+1)
#define WORDENTRY_NEXT_OFFSET   (MAX_WORDLEN+1+4+4)

	.section .rodata
/* Define any string constants or read-only data here */

	.section .text

/*
 * Compute a hash code for the given NUL-terminated
 * character string.
 *
 * The hash algorithm should be implemented as follows:
 *
 * uint32_t hash_code = 5381
 * for each character c of w in order {
 *   hash_code = hash_code * 33 + c
 * }
 *
 * Note that the character values should be treated as
 * being unsigned (in the range 0..255)
 *
 * C function prototype:
 *    uint32_t wc_hash(const unsigned char *w);
 */




	.globl wc_hash
wc_hash:

    subq $8, %rsp /* aligning the stack */
    pushq %r13 /* store the value of r13 */
    pushq %r14 /* store the value of r14 */

    movq $5381, %r13 /* using r13 as hash_code */

    .LhashLoopStart:
        movb (%rdi), %r14b /* storing the first byte of rdi (first char) into r14b */
        cmp $0, %r14b /* checking if char is '\0' */
        je .LhashLoopEnd /* if at '\0', exit loop */

        imulq $33, %r13 /* r13 = r13 * 33 */
        
        movzx %r14b, %r14 /* moving r14b into r14 register with zero extension (i.e. just padding the rest of r14 with zeros) */
        addq %r14, %r13 /* r13 = r13 + r14 */ 

        incq %rdi /* move rdi forward by 1 char */
        jmp .LhashLoopStart /* jump back to the top of the loop */

    .LhashLoopEnd:
        movq %r13, %rax /* putting final value of hash_code into return register */
        popq %r14 /* restoring value of r14 */ 
        popq %r13 /* restoring value of r13 */ 

        add $8, %rsp /* restoring stack pointer */
        ret



/*
 * Compare two strings lexicographically. Return
 *
 * - a negative value if lhs string is less than rhs string
 * - 0 if lhs string is identical to rhs string
 * - a positive value if lhs string is greater than rhs string
 *
 * Lexicographical comparison is a generalization of alphabetical
 * order, but using character codes. If one string is a prefix
 * of the other, it is considered as "less than". E.g.,
 * "hi" would compare as less than "high".
 *
 * C function prototype:
 *    int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs);
 */
	.globl wc_str_compare
wc_str_compare:
    subq $8 ,%rsp /* align the stack pointer */
    pushq %r12 /* preserve value of r12 */
    pushq %r13 /* preserve value of r13 */
    pushq %r14 /* preserve value of r14 */
    pushq %r15 /* preserve value of r15 */

    movq %rdi, %r12 /* store LHS (1st arg) in r12 */
    movq %rsi, %r13 /* store RHS (2nd arg) in r13 */

    .LloopStart:
        movb (%r12), %r14b /* move char at r12 into r14b */
        movb (%r13), %r15b /* move char at r13 into r15b */

        cmp %r14b, %r15b /* compare LHS char at index rbx with RHS char at index rbx */
        je .Lequal /* jump if LHS char = RHS char */
        jg .LlhsLessThanrhs /* jump if LHS char < RHS char */
        jl .LlhsGreaterThanrhs /* jump if LHS char > RHS char */

    
    .Lequal:
        cmpb $0, %r14b /* check if LHS char is NULL terminator (checking if both NULl) */
        je .LloopEndEqual /* if both are null terminator, jump to end of loop */
        addq $1, %r12 /* move LHS pointer (r12) forward to point to the next char */
        addq $1, %r13 /* move RHS pointer (r13) forward to point to the next char */
        jmp .LloopStart /* jump to loop start */

    .LloopEndEqual:
        movl $0, %eax /* move 0 into return register since LHS == RHS */
        jmp .LloopEnd /* jump to loop end */
    
    .LlhsLessThanrhs:
        movl $-1, %eax /* move -1 into return register since LHS < RHS */
        jmp .LloopEnd /* jump to loop end */
    
    .LlhsGreaterThanrhs:
        movl $1, %eax /* move 1 into return register since LHS > RHS */
        jmp .LloopEnd /* jump to loop end */

    .LloopEnd:
        popq %r15 /* restore value of r15 */
        popq %r14 /* restore value of r14 */
        popq %r13 /* restore value of r13 */
        popq %r12 /* restore value of r12 */

        addq $8, %rsp /* restore stack pointer */
        ret

/*
 * Copy NUL-terminated source string to the destination buffer.
 *
 * C function prototype:
 *    void wc_str_copy(unsigned char *dest, const unsigned char *source);
 */
	.globl wc_str_copy
wc_str_copy:
	subq $8, %rsp /* align the stack */

    pushq %r14 /* store the value of r14 */

    .LcopyLoopStart:
        movb (%rsi), %r14b
        movb %r14b, (%rdi) /* copy curr char into destination */
        cmp $0, %r14b 
        je .LcopyLoopEnd /* if curr char is '\0', exit loop */

        addq $1, %rsi /* move source str pointer to next char */
        addq $1, %rdi /* move destination str pointer to next char*/ 
        jmp .LcopyLoopStart

    .LcopyLoopEnd:
        popq %r14 /* restore value of r14 */

        addq $8, %rsp /* restore stack pointer */
        ret

/*
 * Return 1 if the character code in c is a whitespace character,
 * false otherwise.
 *
 * For the purposes of this function, a whitespace character is one of
 *
 *   ' '
 *   '\t'
 *   '\r'
 *   '\n'
 *   '\f'
 *   '\v'
 *
 */
.globl wc_isspace
wc_isspace:
    subq $8, %rsp
    push %r12  /* preserve the value of r12, align the stack */
    movb %dil, %r12b /* move the value in dil (lowest 8 bits of rdi) to r12d */

    cmpb $32, %r12b /* compares char with ASCII value 32 (' ') */
    je .LisSpace

    cmpb $9, %r12b /* compares char with ASCII value 9 ('\t') */
    je .LisSpace

    cmpb $13, %r12b /* compares char with ASCII value 13 ('\r') */
    je .LisSpace

    cmpb $10, %r12b /* compares char with ASCII value 10 ('\n') */
    je .LisSpace

    cmpb $12, %r12b /* compares char with ASCII value 12 ('\f') */
    je .LisSpace

    cmpb $11, %r12b /* compares char with ASCII value 11 ('\v') */
    je .LisSpace

    movl $0, %eax
    jmp .Lloopend

.LisSpace:
    movl $1, %eax
    jmp .Lloopend

.Lloopend:
    popq %r12
    addq $8, %rsp
    ret


/*
 * Return 1 if the character code in c is an alphabetic character
 * ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
 *
 * C function prototype:
 *    int wc_isalpha(unsigned char c);
 */
/* wc_isalpha.S */

; .section .text
.globl wc_isalpha

wc_isalpha:
    /* Function Prologue */
    subq $8, %rsp
    push %r12  /* preserve the value of r12, align the stack */
    movb %dil, %r12b /* move the value in dil (lowest 8 bits of rdi) to r12d */

    /* Check if the character is an alphabetic character */
    cmp $65, %r12b         /* Compare with 'A' (65 in ASCII) */
    jl .LnotAlpha          /* If less than 'A', not alphabetic */

    cmp $90, %r12b         /* Compare with 'Z' (90 in ASCII) */
    jle .LisAlpha          /* If less than or equal to 'Z', it's alphabetic */

    cmp $97, %r12b         /* Compare with 'a' (97 in ASCII) */
    jl .LnotAlpha          /* If less than 'a', not alphabetic */

    cmp $122, %r12b        /* Compare with 'z' (122 in ASCII) */
    jle .LisAlpha          /* If less than or equal to 'z', it's alphabetic */

.LnotAlpha:
    movl $0, %eax         /* Set %eax to 0 (not alphabetic) */
    jmp .Lend

.LisAlpha:
    movl $1, %eax         /* Set %eax to 1 (alphabetic) */

.Lend:
    /* Function Epilogue */
    popq %r12              /* Restore original %r12 value */
    addq $8, %rsp
    ret


/* End of wc_isalpha.S */


/*
 * Read the next word from given input stream, storing
 * the word in the array pointed-to by w. (This array should be
 * assumed to be MAX_WORDLEN+1 elements in size.) Return 1
 * if a word is read successfully, 0 otherwise.
 *
 * For the purposes of this function, a word is a sequence of
 * 1 or more non-whitespace characters.
 *
 * If a sequence of non-whitespace characters has more than
 * MAX_WORDLEN characters, then only the first MAX_WORDLEN
 * characters in the sequence should be stored in the array.
 *
 * C function prototype:
 *    int wc_readnext(FILE *in, unsigned char *w);
 */
	.globl wc_readnext
wc_readnext:
    sub $8,%rsp /* align the stack pointer */
    pushq %r12  /* store the value of r12 */
    pushq %r13  /* store the value of r13 */
    pushq %r14  /* store the value of r14 */
    movq %rsi,%r12  /* store pointer *w in r12 */
    movq %rdi,%r14  /* store file pointer *in in r14 */

    movq $0,%rax 
    movq %r14,%rdi  /* pass *in as arg 1 */
    call fgetc  /* call fgetc(c) function with *in as arg1 */ # how should I convert this to a char?

    movl %eax,%r13d /* move result into r13d (this is our int c)*/
    movq $0,%r10    /* store 0 in r10 (counter i)*/

    .Lrnstartloop:
        cmpl $-1,%r13d    /* check if c == EOF */
        je .Lrnendloop  /* if c < 0 end while loop*/

        movq $0,%rax    
        movl %r13d,%edi /* pass c as argument 1 */
        call wc_isspace /* call wc_isspace with c as arg */

        cmpq $0,%rax    /* check if c is a space */ 
        je .Lcnotspace  /* exit if not a space */
        cmpq $0,%r10    /* check if i == 0 */
        jb .Lrnendloop  /* exit if statement */
        ja .Lrnendloop

        movq $0,%rax    
        movq %r14,%rdi  /* pass *in as arg 1 */
        call fgetc  /* call fgetc(c) function with *in as arg1 */ 

        movl %eax,%r13d   /* store result in int c */
        jmp .Lrnstartloop   /* jump to start of while loop */

    .Lcnotspace:
        movl %r13d,%eax
        movb %al,(%r12)  /* store c as char in w[i] */
        incq %r10   /* increase i by 1 */
        incq %r12
        cmpq $MAX_WORDLEN,%r10  /* check if i == MAX_WORDLEN */
        je .Lrnendloop  /* if equal end while loop */

        movq $0,%rax
        movq %r14,%rdi  /* pass *in as arg 1 */
        call fgetc  /* call fgetc(c) function with *in as arg1 (updates c to have new character) */

        movl %eax,%r13d   /* store result in int c */ 
        jmp .Lrnstartloop   /* jump to start of while loop */

    .Lrnendloop:
        movb $0,(%r12) /* store null pointer (int) as character in w[i] (char) */
        cmpq $0,%r10    /* check if i == 0 */
        je .Lfileisempty    /* if i == 0 jump to empty file */

        movq $1,%rax    /* return 1 if a word was read */
        popq %r12
        popq %r13
        popq %r14
        addq $8,%rsp
        ret

    .Lfileisempty:
        movq $0,%rax    /* return 0 if not words read (empty file) */
        popq %r12
        popq %r13
        popq %r14
        addq $8,%rsp
        ret

/*
 * Convert the NUL-terminated character string in the array
 * pointed-to by w so that every letter is lower-case.
 *
 * C function prototype:
 *    void wc_tolower(unsigned char *w);

 A - 65
 Z - 90
 */
	.globl wc_tolower
wc_tolower: /* seems to do what it needs to - unsure why it's not working. Stepped through teh test it's failling, and it converts and stores properly?? */
	subq $8, %rsp /* align the stack register */

    pushq %r14 /* store value of r14 */

    .LtoLowerLoopStart:
        movb (%rdi), %r14b /* r14b stores the curr char */
        cmpb $0, %r14b /* comparing curr char to null terminator */
        je .LtoLowerLoopEnd /* if curr char is null terminator, exit loop */

        cmpb $65, %r14b /* comparing curr char to A */
        jge .LcharGreaterThanA /* if curr char is >=A, then jump */

        incq %rdi /* if currchar is not above A, then move to next char */
        jmp .LtoLowerLoopStart /* jump back to top of the loop */

    .LcharGreaterThanA:
        cmpb $90, %r14b /* comparing curr char to Z */
        jle .LcapitalChar /* if curr char <= Z (we already know curr char > A), then replace it with the lowercase version */

        incq %rdi /* if curr char is not capital alpha, then move to next char */
        jmp .LtoLowerLoopStart

    .LcapitalChar:
        addb $32, %r14b /* convert curr char to lowercase */
        movb %r14b, (%rdi) /* replace the uppercase char with lowercase char */

        incq %rdi /* move pointer to next char */
        jmp .LtoLowerLoopStart

    .LtoLowerLoopEnd:
        popq %r14 /* restore value of r14 */
        addq $8, %rsp /* realign stack */
        ret

/*
 * Remove any non-alphaabetic characters from the end of the
 * NUL-terminated character string pointed-to by w.
 *
 * C function prototype:
 *    void wc_trim_non_alpha(unsigned char *w);
 */
	.globl wc_trim_non_alpha # not letting me debug this func for some reason, Gonna go to OH to ask about it
wc_trim_non_alpha:
    subq $8,%rsp    /* align the stack register */
    pushq %r12  /* store value of r12 */
    pushq %r13 /* store value of r13 */
	movq $0,%r10 /* initialize length counter to 0 */
    movq %rdi,%r13 /* store pointer *w in r13 */

# getting length of word + null term in *w
.Ltnaloop_start:
    movb (%r13),%r12b /* load the current character from w[i] into %r12b (like char c) */

    cmpb $0,%r12b /* check if it's the null terminator '\0' */
    je .Ltnaloop_end /* if null terminator end loop */

    # Increment i
    incq %r13 /* move character pointer forward by 1 */
    incq %r10 /* increase string length */
    jmp .Ltnaloop_start

.Ltnaloop_end:
    decq %r13 /* set reference *w pointer to last character in word */
    decq %r10 /* length counter start at last element not null terminator */
    movb (%r13),%r12b   /* reset char value to first char in word */
    

.Lcheck_non_alpha:
    cmpq $0,%r10 /* verify i is >= 0 */
    jb .Lexitloop /* if i < 0 exit loop */

    movl $0,%eax
    movb %r12b,%dil /* pass character c as arg 1 */
    call wc_isalpha /* call is alpha */

    cmpl $1,%eax /* return value (int) from isalpha*/
    je .Lexitloop /* if isalpha end loop*/

    movb $0,(%r13) /* add null terminator to non alpha character*/
    decq %r13   /* move character pointer backward 1*/
    decq %r10 /* decrease counter i by 1 */
    movb (%r13),%r12b
    
    jmp .Lcheck_non_alpha 

.Lexitloop:
    popq %r12
    pop %r13
    addq $8,%rsp
    ret

/*
 * Search the specified linked list of WordEntry objects for an object
 * containing the specified string.
 *
 * If a matching object is found, set the int variable pointed-to by
 * inserted to 0 and return a pointer to the matching object.
 *
 * If a matching object is not found, allocate a new WordEntry object,
 * set its next pointer to point to what head points to (i.e., so the
 * new object is at the head of the list), set the variable pointed-to
 * by inserted to 1, and return a pointer to the new node. Note that
 * the new node should have its count value set to 0. (It is the caller's
 * job to update the count.)
 *
 * C function prototype:
 *    struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted);
 */
	.globl wc_find_or_insert
wc_find_or_insert:
    subq $16,%rsp    /* align the stack register */
    pushq %r12 /* save the value of r12 */
    pushq %r13 /* save the value of r13 */
    pushq %r14 /* save the value of r14 */
    pushq %r15 /* save the value of r15 */
    
    movq %rdi,%r12  /* pointer *head */
    movq %rsi,%r13  /* stores pointer *s (word to find) */
    movq %rdx,%r14  /* stores pointer *inserted (int to set) */
    movq %rdi,%r15  /* curr reference to head */

    .Lforistartloop:
        cmpq $0,%r15 /* check if curr is null */ /* CHANGED THIS LINE FROM $0 to $0x0*/
        je .Lforiendloop /* if equal to null end loop */

        movq $0,%rax
        movq %r13,%rdi  /* pass *s as arg 1 */
        movq %r15,%rsi  /* pass curr->word as arg 2 */ /* issue on this line! */
        call wc_str_compare

        cmpq $0,%rax /* check if result is 0 */
        jne .Lwordisntequal

        movl $0,(%r14) /* if word already in list mark inserted 0*/

        movq %r15,%rax /* return the head */
        popq %r15
        popq %r14
        popq %r13
        popq %r12
        addq $16,%rsp
        ret

    .Lwordisntequal:
        movq WORDENTRY_NEXT_OFFSET(%r15),%r15 /* move to next struct WordEntry node */ /* MIGHT BE ERROR ON THIS LINE*/
        jmp .Lforistartloop

    .Lforiendloop:
        movq $0,%rax /* store memory in rax */
        movq  $80,%rdi /* pass size of WordEntry struct */
        call malloc

        movq %rax,%r15 /* read address returned by malloc into r15 */

        /*movq %rax,%rdi /* read address returned by malloc into first arg buffer */

        leaq WORDENTRY_WORD_OFFSET(%r15), %rdi

        movq %r13,%rsi /* read address of source string into 2nd arg */
        call wc_str_copy /* store s in new word node char* */

        /* CHANGED TO movQ for this line from movl!!!*/
        movq $0, WORDENTRY_COUNT_OFFSET(%r15)    /* initialize count to 0 */ 
        movq %r12, WORDENTRY_NEXT_OFFSET(%r15)  /* store address of head WordEntry object in *next */
        
        movq %r15,%rax /* store address of new node into return register */
        movl $1,(%r14) /* store value of 1 in *inserted */
        popq %r15
        popq %r14
        popq %r13
        popq %r12
        addq $16,%rsp
         
        ret
/*
 * Find or insert the WordEntry object for the given string (s), returning
 * a pointer to it. The head of the linked list which contains (or should
 * contain) the entry for s is the element of buckets whose index is the
 * hash code of s mod num_buckets.
 *
 * Returns a pointer to the WordEntry object in the appropriate linked list
 * which represents s.
 *
 * C function prototype:
 *    struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s);
 */
	.globl wc_dict_find_or_insert
wc_dict_find_or_insert:

    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    movq %rdi, %r12 /* moving pointer *buckets[] into r12 */
    movl %esi, %r13d /* moving int num_buckets into r13 */
    movq %rdx, %r14 /* moving string *s into r14 */
    
    /* calculating element index */
    movq %r14, %rdi /* moving string s into first arg register */
    call wc_hash /* calling wc_hash(s) */

    movl $0, %edx /* setting edx to 0 for unsigned division */
    divl %r13d /* divide hash_code (in %rax) by num_buckets */
    movl %edx, %r13d /* store the element index in r13 */

    leaq -8(%rbp), %r15 /* allocate memory on the stack for inserted */
    movq $0, (%r15) /* r15 = inserted. store 0 in inserted */
    leaq (%r12,%r13,8), %rdi /* rdi holds *head = buckets[element_index] */
    movq (%rdi), %rdi
    movq %r14, %rsi /* moving string s into 2nd arg register */
    movq %r15, %rdx /* moving &inserted into 3rd arg register */
    call wc_find_or_insert /* calling wc_find_or_insert(head, s, &inserted) */

    cmpl $1, (%r15) /* 1 == inserted? */
    je .LupdateBuckets /* if yes, then update buckets[element_index] = result */
    jmp .LdictFindInsertDone

    .LupdateBuckets:
        movq %rax, (%r12,%r13,8) /* setting buckets[element_index] = result */
        jmp .LdictFindInsertDone
    
    .LdictFindInsertDone:
        popq %r15
        popq %r14
        popq %r13
        popq %r12
        addq $16, %rsp
        popq %rbp
        ret


/*
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p);
 */
	.globl wc_free_chain
wc_free_chain:

    subq $8,%rsp
	pushq %r12 /* store reference *p to WordEntry object/node */
    pushq %r13 /* store reference *skip to node after current node being removed */
    movq %rdi,%r12

    .Lfreechain:
        cmpq $0,%r12 /* check if no WordEntry node */ 
        je .Lendfreechain
        movq WORDENTRY_NEXT_OFFSET(%r12),%r13 /* store reference to node after node being deleted */
        movq %r12,%rdi /* pass ref of node to be deleted */
        call free

        movq %r13,%r12 /* make *p reference to node to be deleted equal node holding rest of the list */
        jmp .Lfreechain /* free next node */

    .Lendfreechain:
        popq %r13
        popq %r12
        addq $8,%rsp
	    ret

/*
vim:ft=gas:
*/
