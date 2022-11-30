## LL GRAMATIKA

1. <prog> -> <prolog> <seq-stats> <epilog>
2. <prolog> -> <?php <wspace> declare(strict_types=1);
3. <seq-stats> -> <stat> <fnc-decl> <seq-stats>
4. <seq-stats> -> ε
5. <fnc-decl> -> function functionId ( <param> ) : <fnc-type> { <st-list> }
6. <fnc-decl> -> ε
7. <st-list> -> <stat> <st-list>
8. <st-list> -> ε
9. <stat> -> if ( <expr> ) { <st-list> } else { <st-list> }
10. <stat> -> while ( <expr> ) { <st-list> }
11. <stat> -> <assign> ;
12. <stat> -> return <expr> ;
13. <stat> -> return ;
14. <stat> -> ε
15. <fnc-type> -> void
16. <fnc-type> -> <type>
17. <param> -> <type> <var> <params> 
18. <param> -> ε
19. <params> -> , <type> <var> <params>
20. <params> -> ε
21. <type> -> int
22. <type> -> string
23. <type> -> float
24. <type> -> ?int
25. <type> -> ?string
26. <type> -> ?float
27. <assign> -> <expr>
28. <assign> -> <var> <r-side>
29. <r-side> -> <expr>
30. <r-side> -> = <expr>
31. <r-side> -> ε
32. <var> -> $varId
33. <epilog> -> ?>EOF
34. <epilog> -> EOF