/*
 * Assembly language main function implementation
 */

#define MAX_WORDLEN 63
    .section .rodata
    sFileRead: .string "r"
    sOutputFileNotRead: .string "Error: could not open file %s\n"
    sOutputUsage: .string "Usage: %s [filename]\n"
    sPrintTotal: .string "Total words read: %u\n"
    sPrintUnique: .string "Unique words read: %u\n"
    sPrintFrequent: .string "Most frequent word: %s (%u)\n"

	.section .text

	.globl main
main:

    pushq %rbp	/* FILE *in */ # can use this as temp
    movq %rsp,%rbp
	subq $40, %rsp /* align the stack pointer */
    pushq %r12	/* */
    pushq %r13	/* char **argv */ # can use this as temp
    pushq %r14	/* *in file pointer */
    pushq %r15	/* int total words */

    movq %rdi,%r12 
    movq %rsi,%r13


    cmpq $2,%r12    /* check argc == 2 */
    jne .Llessthan3args 
    movq 1(%r13), %rdi /* pass argv[1] as arg 1 */
    movq $sFileRead,%rsi  /* pass string input for fprintf */
    call fopen

    movq %rax,%r14 /* move file address into *in */

    cmpq $0,%r14 /* check if in is null */
    je .Lfilenotread
    
    .Llessthan3args:
     cmpq $1,%r12
     je .Lonly1arg

     movq $sOutputUsage,%rdi
     movq 1(%r13),%rsi
     call printf

     jmp .Lendmain
    
    .Lfilenotread:
     movq stderr,%rdi 
     movq $sOutputFileNotRead,%rsi
     movq 1(%r13),%rdx
     call fprintf /* file output */

    .Lendmain:
     movq $1,%rax
     popq %r15
     popq %r14
     popq %r13
     popq %r12
     addq $8, %rsp
     popq %rbp
     ret

    .Lonly1arg:
     movq $2,%r14
     
    .Lstartmainprogram:
    /* create WordEntry pointer to hashmap */

    movq $13249,%rdi
    call calloc

    movq %rax,%r12 /* r12 now holds hashmap */

    /* create word array */ 
    movq $64,%rdi
    call malloc

    movq %rax,%r13 /* r13 now holds word array */

    movq $0,%r15

    .Lprogramstartloop:
     movq %r14,%rdi
     movq %r13,%rsi
     call wc_readnext

     cmpq $0,%rax
     je .Lendmainwhile
     incq %r15

     movq %r13,%rdi
     call wc_tolower

     movq %r13,%rdi
     call wc_trim_non_alpha

     movq %r12, %rdi
     movq $13249, %rsi
     movq %r13, %rdx
     call wc_dict_find_or_insert

     addl $1,64(%rax) 

     jmp .Lprogramstartloop

    .Lendmainwhile:

    leaq -40(%rbp), %rdi /* rdi temp stores *in */
    movq %r14, (%rdi) /* rdi temp stores *in */


    /* update counters */
     movq $0,%r14 /* forloop i counter*/

     leaq -8(%rbp), %rdi /* -8(%rbp) stores unique_words */
     movq $0, (%rdi) /* unique_words = 0 */

     leaq -16(%rbp), %rdi /* -16(%rbp) stores best_word_count */
     movq $0, (%rdi) /* best_word_count = 0 */

     movq $64,%rdi 
     call calloc /* allocate 64 bytes for best_word */
     leaq -24(%rbp), %rdi /* using rdi as temp to load address of best_word */
     movq %rax, %rdi /* -24(%rbp) is now pointer to best_word */

     leaq -32(%rbp), %rdi /* using rdi as temp */


     .Lforloopcounters:
        cmpq $13249,%r14 /* compare i with HASHTABLE_SIZE */
        jge .Lendforloopcounters

        leaq (%r12, %r14, 4), %rdi /* rdi temp store hastable[i] */
        movq %rdi, -32(%rbp) /* storing entry */

        jmp .LstartWhileLoop

     .LstartWhileLoop:
     /* enter while loop */
     
        leaq -32(%rbp), %rdi /* rdi temp reference to entry */
        cmpq $0, (%rdi) /* compare entry = hashtable[i] with NULL */
        je .LendWhileLoop

        leaq -8(%rbp), %rdi /* temp store unique_words in %rdi */
        addq $1, (%rdi) /* unique_words++ */
        
        leaq -32(%rbp), %rdi /* rdi temp stores entry */
        leaq 64(%rdi), %rdi /* rdi = * entry->count */
        movq (%rdi), %rdi /* rdi stores the int entry->count */

        leaq -16(%rbp), %rsi /* using rsi as temp; *rsi = best_word_count */
        movq (%rsi), %rsi /* rsi = best_word_count */

        cmpq %rsi, %rdi
        jle .LcheckEntryCountEqualBestCount
        leaq -32(%rbp), %rdi /* rdi temp stores entry */
        movq %rdi, -24(%rbp) /* movq entry_word, best_word */
        
        leaq -16(%rbp), %rdi /* rdi temp, rdi = *best_word_count */
        leaq -32(%rbp), %rsi /* rsi temp stores entry */
        movq 64(%rsi), %rdx /* rdx temp stores entry->count */
        movq (%rdx), %rdx
        movq %rdx, (%rdi) /* move entry->count, best_word_count */
        jmp .LwhileLoopBottom

     .LcheckEntryCountEqualBestCount:
        leaq -32(%rbp), %rdi /* rdi temp stores entry */
        leaq 64(%rdi), %rdi /* rdi temp. rdi = *entry->count */
        leaq -16(%rbp), %rsi /* rsi temp. rsi = *best_word_count */
        movq (%rsi), %rsi

        cmpq %rsi, (%rdi) /* cmp best_word_count, entry->count */
        jne .LwhileLoopBottom

        movq -32(%rbp), %rdi /* rdi = entry->word */
        movq -24(%rbp), %rsi /* rsi = *best_word */
        call wc_str_compare
        cmp $0, %rax 
        jge .LwhileLoopBottom /* jump to end of while loop if wc_str_compare(entry->word, best_word) >=0 */
        
        leaq -16(%rbp), %rdi /* rdi temp, rdi = *best_word_count */
        leaq -32(%rbp), %rsi /* rsi temp stores entry */
        leaq 64(%rsi), %rsi /* rsi temp = *entry->count */
        movq (%rsi), %rsi
        movq %rsi, (%rdi) /* move entry->count, best_word_count */
        
        leaq -32(%rbp), %rdi /* rdi temp stores entry */
        movq %rdi, -24(%rbp) /* movq entry_word, best_word */

        jmp .LwhileLoopBottom

     .LwhileLoopBottom:
        /* entry = entry-> next */
        leaq -32(%rbp), %rdi /* rdi temp stores entry */
        movq 72(%rdi), %rdi /* entry = entry->next */
        jmp .LstartWhileLoop /* jump to start of while loop */

     .LendWhileLoop:
        addq $1, %r14 /* i++ */
        jmp .Lforloopcounters /* jump back to start of for loop */

     

     /* finished updating counters */
     .Lendforloopcounters:
        movq $sPrintTotal, %rdi
        movq %r15,%rsi
        call printf

        movq $sPrintUnique, %rdi
        leaq -8(%rbp), %rsi /* rsi temp stores *unique_words */
        movq (%rsi), %rsi /* rsi = unique_words */
        call printf

        movq $sPrintFrequent, %rdi
        leaq -24(%rbp), %rsi /* rsi temp points to best_word */
        leaq -16(%rbp), %rdx /* rdx temp stores *best_word_count */
        movq (%rdx), %rdx /* rdx = best_word_count */
        call printf
        

        leaq -40(%rbp), %rdi
        movq (%rdi), %rdi /* rdi temp stores *in */
        call fclose

    /* clearing hashmap */
    .LforLoopClearChain:
    movq $0, %r14
    movq $0, %r11
    cmpq $13249,%r14
    jge .LfreeChainLoopExit
    movq %r12, %r10
    addq %r11 ,%r10
    movq %r10, %rdi
    call wc_free_chain
    addq $1, %r14
    addq $80, %r11
    jmp .LforLoopClearChain
	
    .LfreeChainLoopExit:
        popq %r15
        popq %r14
        popq %r13
        popq %r12
        addq $40, %rsp
        popq %rbp
        ret

/*
vim:ft=gas:
*/
