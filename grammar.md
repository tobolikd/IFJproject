## LL GRAMATIKA

1. <prog> -> <prolog> <seq-stats> <epilog>
2. <prolog> -> <?php <wspace> declare(strict_types=1);
3. <seq-stats> -> <stat> <seq-stats>
4. <seq-stats> -> <fnc-decl> <seq-stats>
5. <seq-stats> -> ε
6. <fnc-decl> -> function functionId ( <param> ) : <fnc-type> { <st-list> }
7. <st-list> -> <stat> <st-list>
8. <st-list> -> ε
9. <stat> -> if ( <expr> ) { <st-list> } else { <st-list> }
10. <stat> -> while ( <expr> ) { <st-list> }
11. <stat> -> <assign> ;
12. <stat> -> return <expr> ;
13. <stat> -> return ;
14. <fnc-type> -> void
15. <fnc-type> -> <type>
16. <param> -> <type> <var> <params> 
17. <param> -> ε
18. <params> -> , <type> <var> <params>
19. <params> -> ε
20. <type> -> int
21. <type> -> string
22. <type> -> float
23. <type> -> ?int
24. <type> -> ?string
25. <type> -> ?float
26. <assign> -> <expr>
27. <assign> -> <var> <r-side>
28. <r-side> -> = <expr>
29. <r-side> -> ε
30. <var> -> $varId
31. <epilog> -> ?>EOF
32. <epilog> -> EOF